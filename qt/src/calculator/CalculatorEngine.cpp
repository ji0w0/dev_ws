#include "CalculatorEngine.h"

void CalculatorEngine::setFirstOperand(double value)
{
    firstNum = value;
}

void CalculatorEngine::setPendingOperator(const QString &op)
{
    pendingOperator = op;
}

void CalculatorEngine::clearOperator()
{
    pendingOperator.clear();
}

bool CalculatorEngine::hasPendingOperator() const
{
    return !pendingOperator.isEmpty();
}

CalculatorEngine::Result CalculatorEngine::calculate(double secondOperand) const
{
    Result result;

    if (pendingOperator == "+") {
        result.value = firstNum + secondOperand;
    } else if (pendingOperator == "-") {
        result.value = firstNum - secondOperand;
    } else if (pendingOperator == "*") {
        result.value = firstNum * secondOperand;
    } else if (pendingOperator == "/") {
        if (secondOperand == 0.0) {
            result.success = false;
        } else {
            result.value = firstNum / secondOperand;
        }
    } else {
        result.success = false;
    }

    return result;
}