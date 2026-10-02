import serial
import struct
import time
import sys

from PyQt6.QtWidgets import *  # QMainWindow, QApplication, QMessageBox 등 GUI 위젯
from PyQt6.QtGui import *      # PyQt6 GUI 관련 클래스
from PyQt6 import uic          # Qt Designer에서 만든 .ui 파일을 Python에서 읽기
from PyQt6.QtCore import *     # QThread, QTimer, pyqtSignal 등

from_class = uic.loadUiType("Card_reader.ui")[0] 

class Receiver(QThread):
    detected = pyqtSignal(bytes) # 카드가 인식되면 UID(bytes)를 GUI Thread로 전달
    recvTotal = pyqtSignal(int)  # 잔액을 읽으면 int 값을 GUI Thread로 전달
    changedTotal = pyqtSignal()  # 잔액 변경(ST) 완료를 GUI Thread에 알림
    noCard = pyqtSignal()        # Arduino가 NO_TAG(0xFA)를 보내면 GUI Thread에 알림

    def __init__(self, conn, parent=None):      # Receiver 생성 시 시리얼 연결 객체를 전달받음
        super(Receiver, self).__init__(parent)  # QThread 부모 클래스 초기화
        self.is_running = False                 # Thread 반복문의 실행 상태
        self.conn = conn                        # WindowClass와 같은 시리얼 연결 객체를 공유
        print("recv init")                      # Receiver 객체 생성 확인용 출력

    def run(self):                                 # self.recv.start()를 호출하면 별도 Thread에서 자동 실행되는 함수
        print("recv start")                        # 수신 Thread 시작 확인용 출력
        self.is_running = True                     # 수신 반복 시작
        while self.is_running == True:             # stop()이 호출될 때까지 계속 Arduino 응답 확인
            if self.conn.readable():               # 시리얼 연결 객체가 읽기 가능한 상태인지 확인
                res = self.conn.read_until(b'\n')  # Arduino가 '\n'을 보낼 때까지 한 응답 Packet을 byte로 읽음
                if len(res) > 0:                   # 실제 응답이 들어왔을 때만 해석
                    res = res[:-2]                 # Arduino Serial.println()이 붙인 '\r\n' 2 byte 제거
                    cmd = res[:2].decode()         # 응답의 앞 2 byte를 GS/GT/ST 문자열 명령으로 변환

                    if cmd == 'GS' and res[2] == 0:  # GS + STATUS_OK(0x00)이면 카드 인식 성공
                        print("recv detected")       # 카드 인식 응답 확인용 출력
                        self.detected.emit(res[3:])  # 3번째 byte 뒤의 4 byte UID를 GUI Thread로 전달

                    elif cmd == 'GT' and res[2] == 0: # GT + STATUS_OK이면 잔액 읽기 성공
                        print("recvTotal")            # 잔액 응답 확인용 출력
                        print(len(res))               # 수신 Packet 길이 확인용 출력
                        self.recvTotal.emit(int.from_bytes(res[3:7], 'little'))  
                        # 4 byte little-endian 값을 Python int로 복원

                    elif cmd == 'ST' and res[2] == 0:  # ST + STATUS_OK이면 잔액 쓰기 성공
                        print("setTotal")              # 잔액 변경 완료 확인용 출력
                        self.changedTotal.emit()       # GUI에 변경 완료 Signal 전송

                    elif res[2].to_bytes(1, 'little') == b'\xfa':  # 응답 Status가 NO_TAG(0xFA)인지 확인
                        print("recv noCard")                       # 카드가 없는 상태 확인용 출력
                        self.noCard.emit()                         # GUI에 카드 없음 Signal 전송

                    else:                          # 위 조건에 해당하지 않는 Status/응답
                        print("unknown error")     # 예외 응답 표시
                        print(res)                 # 실제 수신 byte를 출력하여 디버깅

    def stop(self):                                # Receiver Thread 반복을 멈추는 함수
        print("recv stop")                         # Thread 종료 확인용 출력
        self.is_running = False                    # run()의 while문 종료

class WindowClass(QMainWindow, from_class):

    def __init__(self):     # GUI 객체가 생성될 때 한 번 실행
        super().__init__()  # QMainWindow 부모 클래스 초기화
        self.setupUi(self)  # card_machine.ui의 위젯을 현재 Window에 배치

        self.uid = bytes(4)
        self.conn = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=1)
        self.recv = Receiver(self.conn)
        self.recv.start()

        self.recv.detected.connect(self.detected)
        self.recv.recvTotal.connect(self.recvTotal)
        self.recv.changedTotal.connect(self.changedTotal)

        self.resetButton.clicked.connect(self.reset)     # 초기화 버튼 클릭 -> reset()
        self.chargeButton.clicked.connect(self.charge)   # 충전 버튼 클릭 -> charge()
        self.paymentButton.clicked.connect(self.payment) # 결제 버튼 클릭 -> payment()

        self.disable()

        self.timer = QTimer()                       # 일정 주기로 카드 상태를 확인할 Timer 생성
        self.timer.setInterval(1000)                # Timer 주기 1000 ms = 1초
        self.timer.timeout.connect(self.getStatus)  # 1초마다 getStatus() 호출
        self.timer.start()                          # 카드가 인식될 때까지 상태 요청 반복 시작

    def enable(self, total):                  # 카드가 인식되고 잔액을 읽었을 때 GUI 활성화
        self.totalLabel.setText(str(total))   # 현재 잔액 Label에 숫자를 문자열로 표시
        self.chargeEdit.setDisabled(False)    # 충전 금액 입력칸 활성화
        self.chargeButton.setDisabled(False)  # 충전 버튼 활성화
        self.paymentEdit.setDisabled(False)   # 결제 금액 입력칸 활성화
        self.paymentButton.setDisabled(False) # 결제 버튼 활성화

    def disable(self):                        # 카드가 없을 때 GUI 입력 기능 비활성화
        self.totalLabel.setText("-")          # 잔액을 '-'로 표시
        self.resetButton.setDisabled(True)    # 초기화 버튼 비활성화
        self.chargeEdit.setDisabled(True)     # 충전 금액 입력칸 비활성화
        self.chargeButton.setDisabled(True)   # 충전 버튼 비활성화
        self.paymentEdit.setDisabled(True)    # 결제 금액 입력칸 비활성화
        self.paymentButton.setDisabled(True)  # 결제 버튼 비활성화

    def send(self, command, data=0):  # Arduino에 명령 Packet을 만들어 보내는 공통 함수
        print("send")                 # 송신 확인용 출력
        # 2B 명령 + 4B UID + 4B int + 1B '\n'을 little-endian Packet으로 생성
        req_data = struct.pack('<2s4sic', command, self.uid, data, b'\n')  
        self.conn.write(req_data)     # 생성한 byte Packet을 Arduino로 전송
        return                        # send() 종료
    
    def getStatus(self):    # GS(Get Status): 카드 존재 여부와 UID 요청
        print("getStatus")  # 함수 호출 확인용 출력
        self.send(b'GS')    # GS 명령 전송, data는 기본값 0
        return

    def getTotal(self):     # GT(Get Total): 현재 카드의 잔액 요청
        print("getTotal")   # 함수 호출 확인용 출력
        self.send(b'GT')    # 저장된 self.uid와 함께 GT 명령 전송
        return

    def recvTotal(self, total):      # GT 응답으로 잔액을 받았을 때 호출
        print("recvTotal")           # 잔액 수신 확인용 출력
        self.enable(total)           # 잔액 표시 + 충전/결제 UI 활성화
        self.chargeEdit.setText("")  # 이전 충전 입력값 삭제
        self.paymentEdit.setText("") # 이전 결제 입력값 삭제
        return

    def setTotal(self, total):   # ST(Set Total): 카드 잔액 변경 요청
        print("setTotal")        # 함수 호출 확인용 출력
        self.send(b'ST', total)  # 새 잔액을 4 byte int로 Packet에 넣어 전송
        return
    
    def changedTotal(self):  # ST 명령이 성공했다는 응답을 받았을 때 호출
        self.getTotal()      # 실제 카드에 기록된 잔액을 다시 읽어 화면 갱신
        return
    
    def detected(self, uid):                # Receiver가 카드 UID를 받았을 때 호출
        print("detected")                   # 카드 감지 확인용 출력
        self.uid = uid                      # 현재 카드 UID 저장
        self.timer.stop()                   # 카드를 찾았으므로 3초 주기 GS 요청 중단
        # self.enable(0)

        self.getTotal()                     # 감지된 카드의 현재 잔액 요청
        self.resetButton.setDisabled(False) # 초기화 버튼은 바로 활성화
        return
    
    def reset(self):
        print("reset")
        self.setTotal(0)
        return
    
    def charge(self):                          # 충전 버튼을 누르면 현재 잔액에 입력 금액을 더함
        print("charge")                        # 충전 동작 확인용 출력
        total = int(self.totalLabel.text())    # 화면에 표시된 현재 잔액 문자열을 int로 변환
        charge = int(self.chargeEdit.text())   # 충전 입력값 문자열을 int로 변환
        self.setTotal(total + charge)          # 현재 잔액 + 충전 금액을 새 잔액으로 기록
        return

    def payment(self):                         # 결제 버튼을 누르면 잔액에서 결제 금액을 차감
        print("payment")                       # 결제 동작 확인용 출력
        total = int(self.totalLabel.text())    # 현재 잔액 문자열을 int로 변환
        payment = int(self.paymentEdit.text()) # 결제 금액 문자열을 int로 변환

        if total < payment:                    # 현재 잔액보다 결제 금액이 크면 결제 불가
            QMessageBox.warning(self, 'Arduino Manager', '잔액이 부족합니다.')  # 경고창 표시
            self.paymentEdit.setText("")       # 결제 입력값 초기화
        else:                                  # 잔액이 충분하면
            total = total - payment            # 잔액에서 결제 금액 차감
            self.setTotal(total)               # 계산된 새 잔액을 카드에 기록

        return                                 # payment() 종료



if __name__ == "__main__":       # 이 파일을 직접 실행할 때만 GUI 시작
    app = QApplication(sys.argv) # PyQt 프로그램 전체 이벤트 루프 객체 생성
    myWindows = WindowClass()    # 메인 Window 객체 생성
    myWindows.show()             # GUI 화면 표시

    sys.exit(app.exec())         # 이벤트 루프 실행, 창을 닫으면 프로그램 종료