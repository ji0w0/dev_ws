#include "MyTimer.h"

MyTimer::MyTimer(QObject *parent) : QThread(parent) {
    interval = 0.1;
}

// timeout 주기 설정 (초 단위)
void MyTimer::setInterval(double sec) {
    interval = sec;
}

// 쓰레드 시작
void MyTimer::start(Priority priority) {
    running = true;
    QThread::start(priority); // 부모 클래스의 start 호출
}

// 쓰레드 실행 루프
void MyTimer::run() {
    while (running) {
        emit timeout(); // timeout 시그널 발생
        
        // time.sleep(interval)에 대응 (밀리초 단위로 변환)
        QThread::msleep(static_cast<unsigned long>(interval * 1000));
    }
}

// 쓰레드 정지
void MyTimer::stop() {
    running = false;
}