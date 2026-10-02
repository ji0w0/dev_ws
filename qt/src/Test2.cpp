#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QtUiTools/QUiLoader>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("Test2.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");

        QPushButton *button1 = myWindow->findChild<QPushButton*>("pushButton");
        QPushButton *button2 = myWindow->findChild<QPushButton*>("pushButton_2");
        QPushButton *button3 = myWindow->findChild<QPushButton*>("pushButton_3");
        
        QLabel *label = myWindow->findChild<QLabel*>("label");

        QLineEdit *lineEdit1 = myWindow->findChild<QLineEdit*>("lineEdit");
        QLineEdit *lineEdit2 = myWindow->findChild<QLineEdit*>("lineEdit_2");
        QLineEdit *lineEdit3 = myWindow->findChild<QLineEdit*>("lineEdit_3");

        int count = 0;
        label->setText(QString::number(count));

        auto increase = [&count, label]() {
            count++;
            label->setText(QString::number(count));
        };
        
        if (button1 && label)
            QObject::connect(button1, &QPushButton::clicked, increase);

        auto reset = [&count, label]() {
            count = 0;
            label->setText(QString::number(count));
        };

        if (button2 && label)
            QObject::connect(button2, &QPushButton::clicked, reset);

        auto summit = [label, lineEdit1]() {
            label->setText(lineEdit1->text());
            lineEdit1->clear();
        };

        if (button3 && label)
            QObject::connect(button3, &QPushButton::clicked, summit);

        auto change = [lineEdit2, lineEdit3]() {
            lineEdit3->setText(lineEdit2->text());
        };

        if (lineEdit2 && lineEdit3) {
            QObject::connect(lineEdit2, &QLineEdit::returnPressed, change);
        }

        // Focus 방지
        lineEdit1->setFocusPolicy(Qt::StrongFocus);
        lineEdit2->setFocusPolicy(Qt::StrongFocus);
        lineEdit3->setFocusPolicy(Qt::StrongFocus);
        button1->setFocusPolicy(Qt::NoFocus);
        button2->setFocusPolicy(Qt::NoFocus);
        button3->setFocusPolicy(Qt::NoFocus);

        myWindow->show();
    }

    return app.exec();
}
