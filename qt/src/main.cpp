#include <QApplication>
#include "TCPClient.h"
 
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TCPClient w;
    w.show();
    return app.exec();
}
