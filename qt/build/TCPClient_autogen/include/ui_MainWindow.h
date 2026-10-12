/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QGroupBox *groupBox;
    QLineEdit *ipEdit;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *portEdit;
    QPushButton *connectButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(651, 145);
        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 20, 601, 91));
        ipEdit = new QLineEdit(groupBox);
        ipEdit->setObjectName("ipEdit");
        ipEdit->setGeometry(QRect(100, 40, 191, 26));
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 40, 81, 18));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(310, 40, 51, 18));
        portEdit = new QLineEdit(groupBox);
        portEdit->setObjectName("portEdit");
        portEdit->setGeometry(QRect(350, 40, 121, 26));
        connectButton = new QPushButton(groupBox);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(490, 40, 88, 26));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Dialog", "GroupBox", nullptr));
        label->setText(QCoreApplication::translate("Dialog", "IP Address", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog", "Port", nullptr));
        connectButton->setText(QCoreApplication::translate("Dialog", "Connect", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
