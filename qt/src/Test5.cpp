#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLineEdit>
#include <QComboBox>
#include <QCalendarWidget>
#include <QtUiTools/QUiLoader>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("Test5.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");
        
        QLineEdit* lineEdit = myWindow->findChild<QLineEdit*>("lineEdit");
        
        QComboBox* comboYear = myWindow->findChild<QComboBox*>("comboBox");
        QComboBox* comboMonth = myWindow->findChild<QComboBox*>("comboBox_2");
        QComboBox* comboDay = myWindow->findChild<QComboBox*>("comboBox_3");

        QCalendarWidget* calendarWidget = myWindow->findChild<QCalendarWidget*>("calendarWidget");

        for (int year = 1900; year <= 2025; ++year) {
            comboYear->addItem(QString::number(year));
        }
        for (int month = 1; month <= 12; ++month) {
            comboMonth->addItem(QString::number(month));
        }
        for (int day = 1; day <= 31; ++day) {
            comboDay->addItem(QString::number(day));
        }

        comboYear->setCurrentText("2000");
        lineEdit->setAlignment(Qt::AlignRight);

        calendarWidget->setMinimumDate(QDate(1900, 1, 1));
        calendarWidget->setMaximumDate(QDate(2025, 12, 31));
        
        auto printBirthDate = [comboYear, comboMonth, comboDay, lineEdit]() {
            QString year = comboYear->currentText();
            QString month = comboMonth->currentText().rightJustified(2, '0');
            QString day = comboDay->currentText().rightJustified(2, '0');
            lineEdit->setText(year + month + day);
        };

        QObject::connect(comboYear, &QComboBox::currentIndexChanged, printBirthDate);
        QObject::connect(comboMonth, &QComboBox::currentIndexChanged, printBirthDate);
        QObject::connect(comboDay, &QComboBox::currentIndexChanged, printBirthDate);

        QObject::connect(calendarWidget, &QCalendarWidget::clicked, [calendarWidget, comboYear, comboMonth, comboDay, lineEdit]() {
            QDate date = calendarWidget->selectedDate();
            QString year = date.toString("yyyy");
            QString month = date.toString("M");
            QString day = date.toString("d");

            comboYear->setCurrentText(year);
            comboMonth->setCurrentText(month);
            comboDay->setCurrentText(day);

            lineEdit->setText(date.toString("yyyyMMdd"));
        });

        myWindow->show();
    }

    return app.exec();
}