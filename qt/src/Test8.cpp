#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QIntValidator>
#include <QtUiTools/QUiLoader>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("Test8.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");

        QSpinBox* spinBox = myWindow->findChild<QSpinBox*>("spinBox");
        QLabel *spinBoxValue = myWindow->findChild<QLabel*>("label");
        
        QLineEdit* editMin = myWindow->findChild<QLineEdit*>("lineEdit");
        QLineEdit* editMax = myWindow->findChild<QLineEdit*>("lineEdit_2");
        QLineEdit* editStep = myWindow->findChild<QLineEdit*>("lineEdit_3");
        QPushButton* applyButton = myWindow->findChild<QPushButton*>("pushButton");

        QSlider* slider = myWindow->findChild<QSlider*>("slider");
        QLabel *sliderValue = myWindow->findChild<QLabel*>("label_2");

        QLabel *labelPixmap = myWindow->findChild<QLabel*>("labelPixmap");
        QPushButton* saveButton = myWindow->findChild<QPushButton*>("pushButton_2");
        QPushButton* openButton = myWindow->findChild<QPushButton*>("pushButton_3");

        QPixmap pixmap;

        spinBox->setAlignment(Qt::AlignRight);
        editMin->setAlignment(Qt::AlignRight);
        editMax->setAlignment(Qt::AlignRight);
        editStep->setAlignment(Qt::AlignRight);
        spinBoxValue->setAlignment(Qt::AlignRight);

        editMin->setValidator(new QIntValidator());
        editMax->setValidator(new QIntValidator());
        editStep->setValidator(new QIntValidator());

        int min = spinBox->minimum();
        int max = spinBox->maximum();
        int step = spinBox->singleStep();

        editMin->setText(QString::number(min));
        editMax->setText(QString::number(max));
        editStep->setText(QString::number(step));

        QObject::connect(applyButton, &QPushButton::clicked, [&min, &max, &step, editMin, editMax, editStep, spinBox, slider](){
            min = editMin->text().toInt();
            max = editMax->text().toInt();
            step = editStep->text().toInt();

            spinBox->setRange(min, max);
            spinBox->setSingleStep(step);

            slider->setRange(min, max);
            slider->setSingleStep(step);
        });

        QObject::connect(spinBox, &QSpinBox::valueChanged, [spinBox, spinBoxValue, slider, &pixmap, labelPixmap](){
            int actualValue = spinBox->value();
            spinBoxValue->setText(QString::number(actualValue));
            slider->setValue(actualValue);

            if (pixmap.isNull())
                return;

            QPixmap new_pixmap = pixmap.scaled(
                pixmap.width(),
                actualValue,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );

            labelPixmap->setPixmap(new_pixmap);
            labelPixmap->resize(new_pixmap.size());
        });

        QObject::connect(slider, &QSlider::valueChanged, [slider, sliderValue, spinBox, &pixmap, labelPixmap](){
            int actualValue = slider->value();
            sliderValue->setText(QString::number(actualValue));
            spinBox->setValue(actualValue);

            if (pixmap.isNull())
                return;

            QPixmap new_pixmap = pixmap.scaled(
                pixmap.width(),
                actualValue,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );

            labelPixmap->setPixmap(new_pixmap);
            labelPixmap->resize(new_pixmap.size());
        });

        QNetworkAccessManager *manager = new QNetworkAccessManager(myWindow);
        QUrl url("https://imageio.forbes.com/specials-images/imageserve/61b1f75e9bdd78e1c08fdd64/A-funny-labrador-dog-with-a-curiously-placed-bubble-in-its-behind-/0x0.jpg?crop=922%2C956%2Cx0%2Cy279%2Csafe&width=960&dpr=1");
        // QUrl url("https://imageio.forbes.com/specials-images/imageserve/61b1f864bc35b3d9629e2148/Funy-cat-Jeff-stealing-the-limelight-from-his-brother-Jaffa-/960x0.jpg?format=jpg&width=1440");
        
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");
        QNetworkReply *reply = manager->get(request);

        QObject::connect(reply, &QNetworkReply::finished, [reply, myWindow, labelPixmap, &pixmap](){
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray imageData = reply->readAll();

                if (pixmap.loadFromData(imageData)) {
                    QPixmap new_pixmap = pixmap.scaled(labelPixmap->width(), labelPixmap->height());
                    labelPixmap->setPixmap(new_pixmap);
                } else {
                    qDebug() << "이미지 디코딩 실패";
                }
            } else {
                qDebug() << "네트워크 에러:" << reply->errorString();
            }
            reply->deleteLater();
        });

        QObject::connect(saveButton, &QPushButton::clicked, [myWindow, &pixmap]() {
            if (pixmap.isNull())
                return;

            QString fileName = QFileDialog::getSaveFileName(
                myWindow,
                "Save File",
                QDir::homePath() + "/untitled.png",
                "*.*"
            );

            if (fileName.isEmpty())
                return;

            if (!pixmap.save(fileName)) {
                QMessageBox::critical(myWindow, "오류", "이미지 저장에 실패했습니다.");
            } else {
                QMessageBox::information(myWindow, "완료", "이미지가 저장되었습니다:\n" + fileName);
            }
        });

        QObject::connect(openButton, &QPushButton::clicked, [myWindow, &pixmap, labelPixmap]() {
            QString fileName = QFileDialog::getOpenFileName(
                myWindow,
                "Save File",
                QDir::homePath(),
                "*.*"
            );

            if (fileName.isEmpty())
                return;

            if (!pixmap.load(fileName)) {
                QMessageBox::critical(myWindow, "오류", "이미지를 불러올 수 없습니다.");
                return;
            }

            QPixmap new_pixmap = pixmap.scaled(labelPixmap->width(), labelPixmap->height());
            labelPixmap->setPixmap(new_pixmap);
        });

        myWindow->show();
    }

    return app.exec();
}