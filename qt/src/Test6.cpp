#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QtUiTools/QUiLoader>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("Test6.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");

        QLineEdit* editName = myWindow->findChild<QLineEdit*>("lineEdit");
        QLineEdit* editGender = myWindow->findChild<QLineEdit*>("lineEdit_2");
        QDateEdit* editBirthday = myWindow->findChild<QDateEdit*>("dateEdit");

        QPushButton* addButton = myWindow->findChild<QPushButton*>("pushButton");
        QTableWidget* table = myWindow->findChild<QTableWidget*>("tableWidget");

        QRegularExpression regx = QRegularExpression("[F|M]");
        QRegularExpressionValidator* gender_validator = new QRegularExpressionValidator(regx, editGender);
        editGender->setValidator(gender_validator);

        table->setColumnCount(3);
        table->setHorizontalHeaderLabels({"Name", "Gender", "BirthDay"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QObject::connect(addButton, &QPushButton::clicked, [table, editName, editGender, editBirthday]() {
            QString name = editName->text();
            QString gender = editGender->text();
            QString birthday = editBirthday->text();
            
            if (name.isEmpty() || gender.isEmpty() || birthday.isEmpty())
                return;

            int row_index = table->rowCount();
            table->insertRow(row_index);
            table->setItem(row_index, 0, new QTableWidgetItem(name));
            table->setItem(row_index, 1, new QTableWidgetItem(gender));
            table->setItem(row_index, 2, new QTableWidgetItem(birthday));
        });
        
        myWindow->show();
    }

    return app.exec();
}