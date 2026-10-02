#pragma once

#include <QString>

class CalculatorEngine
{
public:
    struct Result {
        double value = 0.0;
        bool success = true;
    };

    void setFirstOperand(double value);
    void setPendingOperator(const QString &op);
    void clearOperator();
    bool hasPendingOperator() const;

    Result calculate(double secondOperand) const;

private:
    double firstNum = 0.0;
    QString pendingOperator;
};
