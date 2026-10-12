#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }   // TCPClient.ui 의 최상위 objectName
QT_END_NAMESPACE

class TCPClient : public QDialog
{
    Q_OBJECT

public:
    explicit TCPClient(QWidget *parent = nullptr);
    ~TCPClient() override;

private slots:
    void onConnectClicked();   // connectButton 클릭
    void onMoveClicked();      // moveButton 클릭 (서보 각도 전송)

private:
    // 파이썬의 Struct('@ii') 와 같은 8바이트 패킷 (int 2개)
    struct Packet {
        qint32 pin;
        qint32 status;
    };

    void sendCommand(qint32 pin, qint32 status);   // 파이썬의 send()

    Ui::Dialog *ui;            // .ui 파일에서 만든 위젯들
    QTcpSocket *sock;          // TCP 소켓
};

#endif // TCPCLIENT_H