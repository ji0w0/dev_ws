#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>      // File control definitions (open, O_RDWR 등)
#include <termios.h>    // POSIX terminal control definitions (시리얼 설정)
#include <unistd.h>     // UNIX standard function definitions (read, write, close)
#include <errno.h>

// 시리얼 포트 연결 함수
int connectSerial(const std::string& portName) {
    // Read/Write 모드, 제어 터미널로 지정하지 않음(O_NOCTTY)
    int fd = open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (fd == -1) {
        std::cerr << "Error: Unable to open port " << portName << std::endl;
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    // 보드레이트 설정 (9600 bps)
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    // 제어 옵션 설정 (8N1: 8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;        // 패리티 없음
    options.c_cflag &= ~CSTOPB;        // 스톱 비트 1개
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;            // 데이터 비트 8개
    options.c_cflag |= (CLOCAL | CREAD); // 수신 가능 설정 및 로컬 라인 설정

    // 로컬 옵션 설정 (Raw input mode: 특수 문자의 줄바꿈 처리 등 비활성화)
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // 입력/출력 옵션 설정 (Raw output)
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    // 타임아웃 설정 (timeout = 1초)
    // VMIN = 0, VTIME = 10 (0.1초 단위이므로 10은 1초)
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;

    // 설정 적용
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

// 데이터 수신 함수 (파이썬의 readline() 동작 모사)
std::string readLine(int fd) {
    std::string result;
    char ch;

    while (true) {
        int bytesRead = read(fd, &ch, 1);
        if (bytesRead > 0) {
            if (ch == '\n') {
                break; // 줄바꿈 문자를 만나면 읽기 완료
            }
            if (ch != '\r') { // '\r'은 제외
                result += ch;
            }
        } else {
            // 타임아웃 발생 시 지금까지 읽은 데이터 반환
            break;
        }
    }
    return result;
}

// 메인 루프 함수
void sendAndReceive(int fd) {
    std::string inputData;

    while (true) {
        std::cout << "Input : ";
        std::getline(std::cin, inputData);

        if (inputData == "q") {
            break;
        }

        // 입력값 시리얼 전송
        write(fd, inputData.c_str(), inputData.length());

        // time.sleep(0.1) 대응 (100 밀리초 대기)
        usleep(100000);

        // 시리얼 데이터 수신 및 출력
        std::string recv = readLine(fd);
        if (!recv.empty()) {
            std::cout << "recv : " << recv << std::endl;
        }
    }
}

int main() {
    int conn = connectSerial("/dev/ttyACM0");
    if (conn == -1) {
        return 1;
    }

    sendAndReceive(conn);

    // 시리얼 포트 닫기
    close(conn);
    return 0;
}
