#include "TCPClient.h"
#include "ui_TCPClient.h"   // TCPClient.ui 에서 자동 생성되는 헤더

#include <QAbstractButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QDebug>

TCPClient::TCPClient(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), sock(new QTcpSocket(this))
{
    ui->setupUi(this);   // .ui 파일의 위젯들을 이 창에 배치

    // ---- IP 주소 입력 형식 제한 (정규표현식) ----
    const QString range = R"((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5]))";
    QRegularExpression ipRegex("^" + range + "\\." + range + "\\." +
                               range + "\\." + range + "$");

    ui->ipEdit->setValidator(new QRegularExpressionValidator(ipRegex, this));
    ui->portEdit->setValidator(new QIntValidator(1, 65535, this));
    ui->degreeEdit->setValidator(new QIntValidator(this));   // 정수만 입력

    setWindowTitle("TCP Client");

    // ---- 시그널-슬롯 연결 ----
    connect(ui->connectButton, &QPushButton::clicked,
            this, &TCPClient::onConnectClicked);
    connect(ui->moveButton, &QPushButton::clicked,
            this, &TCPClient::onMoveClicked);

    // LED 버튼: clicked(bool checked) 의 checked 값(켜짐/꺼짐)을 그대로 전송
    connect(ui->led21, &QAbstractButton::clicked, this,
            [this](bool checked) { sendCommand(21, checked); });
    connect(ui->led22, &QAbstractButton::clicked, this,
            [this](bool checked) { sendCommand(22, checked); });
    connect(ui->led23, &QAbstractButton::clicked, this,
            [this](bool checked) { sendCommand(23, checked); });
}

TCPClient::~TCPClient()
{
    sock->close();
    delete ui;
}

void TCPClient::onConnectClicked()
{
    QString ip = ui->ipEdit->text();
    quint16 port = ui->portEdit->text().toUShort();

    sock->abort();                    // 이전 연결 정리
    sock->connectToHost(ip, port);    // ESP32에 접속 시도

    // 최대 3초 동안 접속 결과 대기
    if (sock->waitForConnected(3000)) {
        qDebug() << "connected" << ip << port;
        ui->connectButton->setText("Connected");
        ui->connectButton->setDisabled(true);
    } else {
        qDebug() << "connect failed:" << sock->errorString();
    }
}

void TCPClient::onMoveClicked()
{
    // 입력한 각도를 핀 5번의 값으로 전송
    sendCommand(5, ui->degreeEdit->text().toInt());
}

void TCPClient::sendCommand(qint32 pin, qint32 status)
{
    if (sock->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "not connected";
        return;
    }

    // 1) 8바이트 패킷 전송 (파이썬: self.sock.send(format.pack(pin, status)))
    Packet req{pin, status};
    sock->write(reinterpret_cast<const char *>(&req), sizeof(req));
    sock->flush();

    // 2) 8바이트 응답 수신 (파이썬: self.sock.recv(format.size))
    //    TCP는 쪼개져서 올 수 있으므로 8바이트가 다 모일 때까지 최대 3초 대기
    while (sock->bytesAvailable() < static_cast<qint64>(sizeof(Packet))) {
        if (!sock->waitForReadyRead(3000)) {
            qDebug() << "response timeout";
            return;
        }
    }

    Packet res{};
    sock->read(reinterpret_cast<char *>(&res), sizeof(res));
    qDebug() << "recv:" << res.pin << res.status;   // 파이썬의 print(rev)
}