/********************************************************************************
** Form generated from reading UI file 'TCPClient.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
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
    QGroupBox *groupBox_2;
    QCheckBox *led21;
    QCheckBox *led22;
    QCheckBox *led23;
    QGroupBox *groupBox_3;
    QLabel *label_3;
    QLineEdit *degreeEdit;
    QPushButton *moveButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(651, 290);
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
        groupBox_2 = new QGroupBox(Dialog);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(20, 120, 201, 141));
        led21 = new QCheckBox(groupBox_2);
        led21->setObjectName("led21");
        led21->setGeometry(QRect(20, 40, 92, 24));
        led22 = new QCheckBox(groupBox_2);
        led22->setObjectName("led22");
        led22->setGeometry(QRect(20, 70, 92, 24));
        led23 = new QCheckBox(groupBox_2);
        led23->setObjectName("led23");
        led23->setGeometry(QRect(20, 100, 92, 24));
        groupBox_3 = new QGroupBox(Dialog);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(250, 120, 371, 80));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 40, 101, 18));
        degreeEdit = new QLineEdit(groupBox_3);
        degreeEdit->setObjectName("degreeEdit");
        degreeEdit->setGeometry(QRect(130, 40, 113, 26));
        moveButton = new QPushButton(groupBox_3);
        moveButton->setObjectName("moveButton");
        moveButton->setGeometry(QRect(260, 40, 88, 26));

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
        groupBox_2->setTitle(QCoreApplication::translate("Dialog", "GroupBox", nullptr));
        led21->setText(QCoreApplication::translate("Dialog", "LED 21", nullptr));
        led22->setText(QCoreApplication::translate("Dialog", "LED 22", nullptr));
        led23->setText(QCoreApplication::translate("Dialog", "LED 23", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("Dialog", "Servo Motor", nullptr));
        label_3->setText(QCoreApplication::translate("Dialog", "Target Degree", nullptr));
        moveButton->setText(QCoreApplication::translate("Dialog", "Move", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
