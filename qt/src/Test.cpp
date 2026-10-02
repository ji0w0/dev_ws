#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QtUiTools/QUiLoader>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("Test.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");

        QTextEdit *textEdit = myWindow->findChild<QTextEdit*>("textEdit");
        
        QPushButton *button1 = myWindow->findChild<QPushButton*>("pushButton");
        QPushButton *button2 = myWindow->findChild<QPushButton*>("pushButton_2");
        QPushButton *button3 = myWindow->findChild<QPushButton*>("pushButton_3");

        QRadioButton *radio1 = myWindow->findChild<QRadioButton*>("radioButton");
        QRadioButton *radio2 = myWindow->findChild<QRadioButton*>("radioButton_2");
        QRadioButton *radio3 = myWindow->findChild<QRadioButton*>("radioButton_3");

        QCheckBox *checkBox1 = myWindow->findChild<QCheckBox*>("checkBox");
        QCheckBox *checkBox2 = myWindow->findChild<QCheckBox*>("checkBox_2");
        QCheckBox *checkBox3 = myWindow->findChild<QCheckBox*>("checkBox_3");
        QCheckBox *checkBox4 = myWindow->findChild<QCheckBox*>("checkBox_4");

        QCheckBox *checkBox5 = myWindow->findChild<QCheckBox*>("checkBox_5");
        QCheckBox *checkBox6 = myWindow->findChild<QCheckBox*>("checkBox_6");
        QCheckBox *checkBox7 = myWindow->findChild<QCheckBox*>("checkBox_7");
        QCheckBox *checkBox8 = myWindow->findChild<QCheckBox*>("checkBox_8");

        auto clickButton1 = [textEdit, radio1]() {
            textEdit->setText("Button 1");
            radio1->setChecked(true);
        };
        auto clickButton2 = [textEdit, radio2]() {
            textEdit->setText("Button 2");
            radio2->setChecked(true);
        };
        auto clickButton3 = [textEdit, radio3]() {
            textEdit->setText("Button 3");
            radio3->setChecked(true);
        };

        if (button1 && button2 && button3 && radio1 && radio2 && radio3 && textEdit) {
            QObject::connect(button1, &QPushButton::clicked, clickButton1);
            QObject::connect(button2, &QPushButton::clicked, clickButton2);
            QObject::connect(button3, &QPushButton::clicked, clickButton3);
        }

        auto clickRadio = [radio1, radio2, radio3, textEdit]() {
            if (radio1->isChecked())      textEdit->setText("Radio 1");
            else if (radio2->isChecked()) textEdit->setText("Radio 2");
            else if (radio3->isChecked()) textEdit->setText("Radio 3");
            else                          textEdit->setText("Unknown");
        };

        if (radio1 && radio2 && radio3 && textEdit) {
            QObject::connect(radio1, &QRadioButton::clicked, clickRadio);
            QObject::connect(radio2, &QRadioButton::clicked, clickRadio);
            QObject::connect(radio3, &QRadioButton::clicked, clickRadio);
        }

        auto clickCheckBox1 = [textEdit, checkBox1, checkBox5]() {
            textEdit->setText("CheckBox 1");
            checkBox5->setChecked(checkBox1->isChecked());
        };
        auto clickCheckBox2 = [textEdit, checkBox2, checkBox6]() {
            textEdit->setText("CheckBox 2");
            checkBox6->setChecked(checkBox2->isChecked());
        };
        auto clickCheckBox3 = [textEdit, checkBox3, checkBox7]() {
            textEdit->setText("CheckBox 3");
            checkBox7->setChecked(checkBox3->isChecked());
        };
        auto clickCheckBox4 = [textEdit, checkBox4, checkBox8]() {
            textEdit->setText("CheckBox 4");
            checkBox8->setChecked(checkBox4->isChecked());
        };

        if (checkBox1 && checkBox2 && checkBox3 && checkBox4 && checkBox5 && checkBox6 && checkBox7 && checkBox8 && textEdit) {
            QObject::connect(checkBox1, &QCheckBox::clicked, clickCheckBox1);
            QObject::connect(checkBox2, &QCheckBox::clicked, clickCheckBox2);
            QObject::connect(checkBox3, &QCheckBox::clicked, clickCheckBox3);
            QObject::connect(checkBox4, &QCheckBox::clicked, clickCheckBox4);

            checkBox5->setAttribute(Qt::WA_TransparentForMouseEvents, true); // mouse
            checkBox5.setFocusPolicy(Qt.FocusPolicy.NoFocus) // keyboard
            checkBox6->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            checkBox6.setFocusPolicy(Qt.FocusPolicy.NoFocus)
            checkBox7->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            checkBox7.setFocusPolicy(Qt.FocusPolicy.NoFocus)
            checkBox8->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            checkBox8.setFocusPolicy(Qt.FocusPolicy.NoFocus)
        }
        
        myWindow->show();
    }

    return app.exec();
}
