import sys
from PyQt6.QtWidgets import *
from PyQt6.QtGui import *
from PyQt6 import uic
from PyQt6.QtCore import *

import serial
import struct

from_class = uic.loadUiType("card_machine.ui")[0]
class WindowClass(QMainWindow, from_class) :
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        self.uid = bytes(4)
        self.conn = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=1)
        self.recv = Receiver(self.conn) # 수신 전용 백그라운드 스레드
        self.recv.start()

        self.recv.detected.connect(self.detected)
        self.recv.recvTotal.connect(self.recvTotal)
        self.recv.changedTotal.connect(self.changedTotal)

        self.resetButton.clicked.connect(self.reset)
        self.chargeButton.clicked.connect(self.charge)
        self.paymentButton.clicked.connect(self.payment)

        self.disable()

        # Tag가 인식될 때까지 GS 반복 요청
        self.timer = QTimer()
        self.timer.setInterval(3000)
        self.timer.timeout.connect(self.getStatus)
        self.timer.start()


    # ---------- UI 활성화 / 비활성화 ----------
    def enable(self, total):
        self.totalLabel.setText(str(total))
        self.chargeEdit.setDisabled(False)
        self.chargeButton.setDisabled(False)
        self.paymentEdit.setDisabled(False)
        self.paymentButton.setDisabled(False)

    def disable(self):
        self.totalLabel.setText("-")
        self.resetButton.setDisabled(True)
        self.chargeEdit.setDisabled(True)
        self.chargeButton.setDisabled(True)
        self.paymentEdit.setDisabled(True)
        self.paymentButton.setDisabled(True)

    # ---------- 버튼 핸들러 ----------
    def reset(self):
        print("reset")
        self.setTotal(0)
        return

    def charge(self):
        print("charge")
        return

    def payment(self):
        print("payment")
        return

    # ---------- 시리얼 송신 ----------
    def send(self, command, data=0):
        print('[DEBUG] send')
        # struct와 형태가 일치하는 11bytes 고정 크기의 이진 패킷으로 압축 (< : 리틀 엔디언)
         # 명령어 2bytes + 카드 UID 4bytes + 충전/결제 금액 등 4bytes + 개행 문자 1byte
        req_data = struct.pack('<2s4sic', command, self.uid, data, b'\n')
        self.conn.write(req_data); # 전송
        return 

    # ---------- 명령어 (송신) ----------
    def getStatus(self): # 명령어 GS
        print("[COMMAND] getStatus")
        self.send(b'GS')
        return

    def getTotal(self): # 명령어 GT
        print("[COMMAND] getTotal")
        self.send(b'GT')
        return

    def setTotal(self, total): # 명령어 ST
        print("[COMMAND] setTotal")
        self.send(b'ST', total)
        return

    # ---------- 시그널 슬롯 (수신 스레드에서 호출됨) ----------
    def detected(self, uid): # 태그가 인식 되었을 때 (GS)
        print('[SIGNAL] detected')
        self.uid = uid
        self.timer.stop()
        # self.enable(0)

        self.getTotal()
        self.resetButton.setDisabled(False)
        return
    
    def recvTotal(self, total): # 잔액 값을 받았을 때 (GT)
        print('[SIGNAL] recvTotal')
        self.enable(total)
        self.chargeEdit.setText('')
        self.paymentEdit.setText('')
    
    def changedTotal(self): # 잔액이 변경 되었을 때 (ST)
        self.getTotal()
        return

# ============================================================
# 수신 스레드: 시리얼로 들어오는 응답을 계속 읽어서 시그널로 전달
# ============================================================
class Receiver(QThread):
    detected = pyqtSignal(bytes)
    recvTotal = pyqtSignal(int)
    changedTotal = pyqtSignal()

    def __init__(self, conn, parent=None):
        super(Receiver, self).__init__(parent)
        self.is_running = False
        self.conn = conn
        print("[DEBUG] recv init")

    def run(self):
        print("[DEBUG] recv start")
        self.is_running = True
        while (self.is_running == True):
            if self.conn.readable():
                res = self.conn.read_until(b'\n')
                if len(res) > 0:
                    res = res[:-2]
                    cmd = res[:2].decode()
                    if cmd == 'GS' and res[2] == 0: # Get Status 수신
                        print('[DEBUG] recv detected')
                        self.detected.emit(res[3:])
                    elif cmd == 'GT' and res[2] == 0: # Get Total 수신
                        print('recvTotal')
                        print(len(res))
                        self.recvTotal.emit(int.from_bytes(res[3:7], 'little'))
                    elif cmd == 'ST' and res[2] == 0: # Set Total 수신
                        print('setTotal')
                        self.changedTotal.emit()
                    else:
                        print('[DEBUG] unknown error')
                        print(cmd)

    def stop(self):
        print("recv stop")
        self.is_running = False

if __name__ == "__main__":
    app = QApplication(sys.argv)
    myWindows = WindowClass()
    myWindows.show()

    sys.exit(app.exec())