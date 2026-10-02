#pragma once

#include "CalculatorEngine.h"

#include <QDialog>

// 전방 선언 (헤더 경량화)
class QLineEdit;
class QPushButton;

class WindowClass : public QObject
{
    Q_OBJECT

public:
    explicit WindowClass(QWidget *parent = nullptr);
    ~WindowClass() override = default;
    void show();

private:
    void setupUi();
    void setupConnections();
    void calculateResult();

    QDialog *dialog = nullptr;

    // UI 위젯
    QLineEdit *lineEdit{nullptr};
    QPushButton *allClearButton{nullptr};
    QPushButton *clearEntryButton{nullptr};
    QPushButton *equalButton{nullptr};
    QPushButton *dotButton{nullptr};
    QPushButton *signButton{nullptr};
    QPushButton *numberButtons[10]{nullptr};
    QPushButton *operatorButtons[4]{nullptr}; // +, -, *, /

    CalculatorEngine engine;

    bool isWaitingForNextNum{false};
};
