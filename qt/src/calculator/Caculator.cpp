#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>
#include <QtUiTools/QUiLoader>

#include "./WindowClass.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    WindowClass window;
    window.show();

    return app.exec();
}