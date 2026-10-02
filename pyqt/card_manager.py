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
        self.recv = Receiver(self.conn)
        self.recv.start() # 스레드 시작

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
    
    def send(self, command, data=0):
        print('[DEBUG] send')
        # struct와 형태가 일치하는 11bytes 고정 크기의 이진 패킷으로 압축 (< : 리틀 엔디언)
        req_data = struct.pack('<2s4sic', command, self.uid, data, b'\n') # 명령어 2bytes + 카드 UID 4bytes + 충전/결제 금액 등 4bytes + 개행 문자 1byte
        self.conn.write(req_data); # 전송
        return 

    def getStatus(self): # 명령어 GS
        print("[COMMAND] getStatus")
        self.send(b'GS')
        return

    def detected(self, uid): # tag detected
        print('[SIGNAL] detected')
        self.uid = uid
        self.timer.stop()
        # self.enable(0)

        self.getTotal()
        self.resetButton.setDisabled(False)
        return

    def getTotal(self): # 명령어 GT
        print("[COMMAND] getTotal")
        self.send(b'GT')
        return

    def recvTotal(self, total):
        print('[SIGNAL] recvTotal')
        self.enable(total)
        self.chargeEdit.setText('')
        self.paymentEdit.setText('')

    def setTotal(self, total): # 명령어 ST
        print("[COMMAND] setTotal")
        self.send(b'ST', total)
        return

    def changedTotal(self):
        self.getTotal()
        return

class Receiver(QThread): # 백그라운드 수신 스레드
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
                        #######################
                    elif cmd == 'GS':
                        if res[2] == 0:
                            print('recv detected')
                            self.detected.emit(res[3:])
                        elif res[2] == 0xFA:
                            print('[INFO] 카드가 감지되지 않음 (태그 대기 중...)')
                        else:
                            print(f'[ERROR] 기타 오류 상태 코드: {hex(res[2])}')
                        #######################
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