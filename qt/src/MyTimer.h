#ifndef MYTIMER_H
#define MYTIMER_H

#include <QThread>

class MyTimer : public QThread {
    Q_OBJECT

signals:
    void timeout(); // timeout 시그널 선언

public:
    explicit MyTimer(QObject *parent = nullptr);

    void setInterval(double sec);
    void start(Priority priority = InheritPriority); // start 함수 오버라이드
    void stop();

protected:
    void run() override; // 쓰레드 실행 루프

private:
    double interval = 0.1;
    bool running = false;
};

#endif // MYTIMER_H