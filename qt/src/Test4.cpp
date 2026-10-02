#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QInputDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QtUiTools/QUiLoader>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("Test4.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");    
        
        QTextEdit *textEdit = myWindow->findChild<QTextEdit*>("textEdit");
        
        QPushButton *pushButton1 = myWindow->findChild<QPushButton*>("pushButton");
        QPushButton *pushButton2 = myWindow->findChild<QPushButton*>("pushButton_2");
        QPushButton *pushButton3 = myWindow->findChild<QPushButton*>("pushButton_3");
        QPushButton *pushButton4 = myWindow->findChild<QPushButton*>("pushButton_4");
        QPushButton *pushButton5 = myWindow->findChild<QPushButton*>("pushButton_5");
        
        QLineEdit *lineEdit = myWindow->findChild<QLineEdit*>("lineEdit");

        textEdit->setReadOnly(true);
        
        QObject::connect(pushButton1, &QPushButton::clicked, [myWindow, textEdit]() {
            bool ok = false;
            QString text = QInputDialog::getText(myWindow,
                                                "QInputDialog - Name", 
                                                "User name:", 
                                                QLineEdit::Normal, 
                                                "", 
                                                &ok);
            if (ok && !text.isEmpty()) {
                textEdit->append(text);
            }
        });

        QObject::connect(pushButton2, &QPushButton::clicked, [myWindow, textEdit]() {
            QStringList items;
            items << "Spring" << "Summer" << "Fall" << "Winter";
            
            bool ok = false;
            QString item = QInputDialog::getItem(myWindow,
                                                "QInputDialog - Seoson", 
                                                "Seasons:", 
                                                items, 
                                                0,
                                                false, 
                                                &ok);
            if (ok && !item.isEmpty()) {
                textEdit->append(item);
            }
        });

        QObject::connect(pushButton3, &QPushButton::clicked, [myWindow, textEdit]() {
            QColor color = QColorDialog::getColor(Qt::white, myWindow, "Select Color");
            
            if (color.isValid()) {
                textEdit->append("Color");
                textEdit->selectAll();
                textEdit->setTextColor(color);

                // 26번 라인: 커서를 맨 끝으로 이동 (선택 해제)
                QTextCursor cursor = textEdit->textCursor();
                cursor.movePosition(QTextCursor::End);
                textEdit->setTextCursor(cursor);
            }
        });

        QObject::connect(pushButton4, &QPushButton::clicked, [myWindow, textEdit]() {
            bool ok = false;
            QFont font = QFontDialog::getFont(&ok, myWindow);

            if (ok) {
                QFontInfo info(font);
                textEdit->append(info.family() + " " + info.styleName());
                textEdit->selectAll();
                textEdit->setCurrentFont(font);

                QTextCursor cursor = textEdit->textCursor();
                cursor.movePosition(QTextCursor::End);
                textEdit->setTextCursor(cursor);
            }
        });

        QObject::connect(pushButton5, &QPushButton::clicked, [myWindow, textEdit]() {
            QString fileName = QFileDialog::getOpenFileName(myWindow, "Open File", "./");
            if (!fileName.isEmpty()) {
                QFile file(fileName);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    QString data = in.readAll();
                    file.close();
                    textEdit->append(data);
                }
            }
        });

        QObject::connect(lineEdit, &QLineEdit::returnPressed, [myWindow, lineEdit, textEdit]() {
            QString text = lineEdit->text();
            bool isNumber = false;
            text.toInt(&isNumber);
            if (isNumber) {
                textEdit->append(text);
            } 
            else {
                QMessageBox::warning(myWindow, 
                                    "QMessageBox - setText", 
                                    "Please enter only numbers.");
            }
            lineEdit->clear();
        });

        myWindow->show();
    }

    return app.exec();
}