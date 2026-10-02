#include "./WindowClass.h"

#include <QFile>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QtUiTools/QUiLoader>

WindowClass::WindowClass(QWidget *parent) : QObject(parent) {
    setupUi();
    setupConnections();
}

void WindowClass::show()
{
    if (dialog)
        dialog->show();
}

void WindowClass::setupUi()
{
    QUiLoader loader;
    QFile file(":/Caculator.ui");
    if (!file.open(QFile::ReadOnly)) {
        return;
    }

    QWidget *loadedWidget = loader.load(&file);
    file.close();

    if (loadedWidget) {
        dialog = qobject_cast<QDialog*>(loadedWidget);
        loadedWidget->setWindowTitle("Calculator"); 

        lineEdit = loadedWidget->findChild<QLineEdit*>("lineEdit");
        allClearButton = loadedWidget->findChild<QPushButton*>("allClearButton");
        clearEntryButton = loadedWidget->findChild<QPushButton*>("clearEntryButton");
        equalButton = loadedWidget->findChild<QPushButton*>("equalButton");
        dotButton = loadedWidget->findChild<QPushButton*>("dotButton");
        signButton = loadedWidget->findChild<QPushButton*>("signButton");

        for (int i = 0; i < 10; ++i) {
            QString objectName = "numberButton" + QString::number(i);
            numberButtons[i] = loadedWidget->findChild<QPushButton*>(objectName);
        }

        for (int i = 0; i < 4; ++i) {
            QString objectName = "operatorButton" + QString::number(i + 1);
            operatorButtons[i] = loadedWidget->findChild<QPushButton*>(objectName);
        }
    }

    if (lineEdit) {
        lineEdit->setReadOnly(true);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setText("0");
    }
}

void WindowClass::setupConnections()
{
    if (allClearButton) {
        connect(allClearButton, &QPushButton::clicked, this, [this]() {
            lineEdit->setText("0");
            engine.setFirstOperand(0.0);
            engine.clearOperator();
            isWaitingForNextNum = false;
        });
    }

    if (clearEntryButton) {
        connect(clearEntryButton, &QPushButton::clicked, this, [this]() {
            QString text = lineEdit->text();
            text.chop(1);
            if (text.isEmpty()) {
                text = "0";
            }
            lineEdit->setText(text);
        });
    }

    for (int i = 0; i < 10; ++i) {
        if (numberButtons[i]) {
            connect(numberButtons[i], &QPushButton::clicked, this, [this, i]() {
                if (lineEdit->text() == "0" || isWaitingForNextNum) {
                    lineEdit->setText(QString::number(i));
                    isWaitingForNextNum = false;
                } else {
                    lineEdit->setText(lineEdit->text() + QString::number(i));
                }
            });
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (operatorButtons[i]) {
            connect(operatorButtons[i], &QPushButton::clicked, this, [this, i]() {
                engine.setFirstOperand(lineEdit->text().toDouble());
                engine.setPendingOperator(operatorButtons[i]->text());
                isWaitingForNextNum = true;
            });
        }
    }

    if (equalButton) {
        connect(equalButton, &QPushButton::clicked, this, &WindowClass::calculateResult);
    }

    if (dotButton) {
        connect(dotButton, &QPushButton::clicked, this, [this]() {
            QString text = lineEdit->text();

            if (isWaitingForNextNum) {
                lineEdit->setText("0.");
                isWaitingForNextNum = false;
                return;
            }

            if (text.contains('.')) {
                return;
            }

            lineEdit->setText(text + ".");
        });
    }

    if (signButton) {
        connect(signButton, &QPushButton::clicked, this, [this]() {
            QString text = lineEdit->text();
            
            bool isNumber = false;
            text.toDouble(&isNumber);
            if (!isNumber)
                return;

            if (text == "0" || text == "0.")
                return;

            if (text.startsWith('-')) {
                text.remove(0, 1);       // - -> +
            } else {
                text.prepend('-');       // + -> -
            }

            lineEdit->setText(text);
        });
    }
}

void WindowClass::calculateResult()
{
    if (!engine.hasPendingOperator())
        return;

    double secondNum = lineEdit->text().toDouble();
    CalculatorEngine::Result result = engine.calculate(secondNum);

    if (!result.success) {
        lineEdit->setText("Error");
        engine.clearOperator();
        isWaitingForNextNum = true;
        return;
    }

    lineEdit->setText(QString::number(result.value));
    engine.clearOperator();
    isWaitingForNextNum = true;
}
