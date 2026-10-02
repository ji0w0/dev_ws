#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QIntValidator>
#include <QtUiTools/QUiLoader>
#include <Qt>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("Test3.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!"); 
        
        QTextEdit *inputText = myWindow->findChild<QTextEdit*>("inputText");
        QTextEdit *outputText = myWindow->findChild<QTextEdit*>("outputText");
        
        QPushButton *addButton = myWindow->findChild<QPushButton*>("addButton");
        QPushButton *clearButton = myWindow->findChild<QPushButton*>("clearButton");

        QPushButton *fontButton1 = myWindow->findChild<QPushButton*>("fontButton");
        QPushButton *fontButton2 = myWindow->findChild<QPushButton*>("fontButton_2");

        QPushButton *colorButton1 = myWindow->findChild<QPushButton*>("colorButton");
        QPushButton *colorButton2 = myWindow->findChild<QPushButton*>("colorButton_2");
        QPushButton *colorButton3 = myWindow->findChild<QPushButton*>("colorButton_3");

        QLineEdit *fontSizeLine = myWindow->findChild<QLineEdit*>("fontSizeLine");
        QPushButton *fontSizeButton = myWindow->findChild<QPushButton*>("fontSizeButton");
        
        if (addButton && inputText && outputText) {
            auto addText = [inputText, outputText]() {
                outputText->append(inputText->toPlainText());
                inputText->clear();
            };
            QObject::connect(addButton, &QPushButton::clicked, addText);
        }

        if (clearButton && outputText) {
            auto clearText = [outputText]() {
                outputText->clear();
            };
            QObject::connect(clearButton, &QPushButton::clicked, clearText);
        }

        if (fontButton1 && fontButton2 && outputText) {
            auto setFont = [outputText](const QString& fontName) {
                QFont font(fontName, 11);
                outputText->setFont(font);
            };
            QObject::connect(fontButton1, &QPushButton::clicked, [setFont]() { setFont("Ubuntu"); });
            QObject::connect(fontButton2, &QPushButton::clicked, [setFont]() { setFont("NanumGothic"); });
        }

        if (colorButton1 && colorButton2 && colorButton3 && outputText) {
            auto setTextColor = [outputText](const int r, const int g, const int b) {
                QColor color = QColor(r, g, b);
                outputText->selectAll();
                outputText->setTextColor(color);
                outputText->moveCursor(QTextCursor::End);
            };
            QObject::connect(colorButton1, &QPushButton::clicked, [setTextColor]() { setTextColor(255, 0, 0); });
            QObject::connect(colorButton2, &QPushButton::clicked, [setTextColor]() { setTextColor(0, 255, 0); });
            QObject::connect(colorButton3, &QPushButton::clicked, [setTextColor]() { setTextColor(0, 0, 255); });
        }

        if (fontSizeLine && fontSizeButton && outputText) {
            fontSizeLine->setValidator(new QIntValidator(1, 100, fontSizeLine)); // 숫자만 입력
        auto setTextSize = [fontSizeLine, outputText]() {
                int size = fontSizeLine->text().toInt();
                outputText->selectAll();
                outputText->setFontPointSize(size);
                outputText->moveCursor(QTextCursor::End);
                fontSizeLine->clear();
            };
            QObject::connect(fontSizeButton, &QPushButton::clicked, setTextSize);
            QObject::connect(fontSizeLine, &QLineEdit::returnPressed, setTextSize);
            fontSizeLine->setAlignment(Qt::AlignRight);
        }

        myWindow->show();
    }

    return app.exec();
}