import serial
import time

def connect():
    return serial.Serial('/dev/ttyACM0', 9600, timeout=1) # 아두이노와 연결

def send(conn):
    while True:
        data = input("Input : ")
        if (data == 'q'):
            break;

        conn.write(data.encode()) # 입력값을 시리얼 통신에 필요한 bytes 형태로 변환 후 전송
        time.sleep(0.1)

        if (conn.readable()):
            recv = conn.readline().decode().strip('\r\n')
            if (len(recv) > 0): # 수신된 데이터가 있을 때
                print( "recv : " + str(recv))
    return;

if __name__ == "__main__":
    conn = connect()
    send(conn)