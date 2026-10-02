#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QtUiTools/QUiLoader>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("HelloQt.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");        
        myWindow->show();
    }

    return app.exec();
}