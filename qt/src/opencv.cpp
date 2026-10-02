#include "./MyTimer.h"

#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QDateTime>
#include <QPushButton>
#include <QFileDialog>
#include <QtUiTools/QUiLoader>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file("opencv.ui");
    
    if (!file.open(QFile::ReadOnly)) {
        return -1;
    }

    QWidget *myWindow = loader.load(&file);
    file.close();

    if (myWindow) {
        myWindow->setWindowTitle("Hello, Qt!");        
        
        QLabel *label = myWindow->findChild<QLabel*>("label");
        QLabel *label2 = myWindow->findChild<QLabel*>("label_2");

        QPushButton *btnOpen = myWindow->findChild<QPushButton*>("pushButton");
        QPushButton *btnCamera = myWindow->findChild<QPushButton*>("pushButton_2");
        QPushButton *btnRecord = myWindow->findChild<QPushButton*>("pushButton_3");
        QPushButton *btnCapture = myWindow->findChild<QPushButton*>("pushButton_4");
        
        QPixmap pixmap = QPixmap();
        bool cameraOn = false;
        bool recording = false;
        int count = 0;
        
        cv::VideoCapture video;
        MyTimer *camera_timer = new MyTimer();
        camera_timer->setInterval(0.1);

        cv::VideoWriter writer;
        MyTimer *record_timer = new MyTimer();
        record_timer->setInterval(0.1);

        btnRecord->hide();
        btnCapture->hide();

        cv::Mat image;
        QObject::connect(btnOpen, &QPushButton::clicked, [myWindow, label, &pixmap, &image](){
            QString fileName = QFileDialog::getOpenFileName(
                myWindow, 
                "Open Image", 
                "", 
                "Image (*.png *.jpg)"
            );

            if (fileName.isEmpty())
                return;

            image = cv::imread(fileName.toStdString());
            if (image.empty()) return;
            
            cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

            QImage qimage(
                image.data, 
                image.cols, 
                image.rows, 
                static_cast<int>(image.step), 
                QImage::Format_RGB888
            );

            pixmap = QPixmap::fromImage(qimage);
            pixmap = pixmap.scaled(
                label->width(), 
                label->height(), 
                Qt::KeepAspectRatio // (선택) 이미지 비율 유지 옵션
            );

            label->setPixmap(pixmap);
        });

        QObject::connect(btnCamera, &QPushButton::clicked, [&cameraOn, btnCamera, &video, camera_timer, btnRecord, btnCapture](){
            if (cameraOn == false) {
                btnCamera->setText("Camera off");
                cameraOn = true;
                camera_timer->start();
                video.open(-1);
                btnRecord->show();
                btnCapture->show();
            } else {
                btnCamera->setText("Camera on");
                cameraOn = false;
                camera_timer->stop();
                video.release();
                btnRecord->hide();
                btnCapture->hide();
            }
        });

        QObject::connect(camera_timer, &MyTimer::timeout, [&video, &pixmap, label, &image](){
            bool retval = video.read(image); 
            if (retval && !image.empty()) {
                cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

                QImage qimage(
                    image.data, 
                    image.cols, 
                    image.rows, 
                    static_cast<int>(image.step), 
                    QImage::Format_RGB888
                );

                pixmap = QPixmap::fromImage(qimage);
                pixmap = pixmap.scaled(
                    label->width(), 
                    label->height()
                );

                label->setPixmap(pixmap);
            }
        });

        auto startRecording = [&writer, record_timer, &recording, &video]() {
            // 1. 현재 날짜/시간 기반 파일명 생성 (%Y%m%d_%H%M%S 대응)
            QString now = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
            std::string filename = now.toStdString() + ".avi";

            // 2. fourcc 코덱 설정 ("*XVID" 대응)
            int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');

            // 3. 웹캠 프레임 가로/세로 크기 가져오기
            int w = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
            int h = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));

            // 4. VideoWriter 객체 생성 및 열기
            writer.open(filename, fourcc, 20.0, cv::Size(w, h));

            // 5. 녹화 타이머 시작
            record_timer->start();
            recording = true;
        };

        auto stopRecording = [&writer, record_timer, &recording]() {
            // 녹화 타이머 정지
            record_timer->stop();

            // 녹화 중일 경우 writer 해제
            if (recording) {
                writer.release();
                recording = false;
            }
        };

        QObject::connect(btnRecord, &QPushButton::clicked, [&recording, btnRecord, record_timer, startRecording, stopRecording](){
            if (recording == false) {
                btnRecord->setText("Rec Stop");
                recording = true;
                record_timer->start();
                startRecording();
            } else {
                btnRecord->setText("Rec Start");
                recording = false;
                record_timer->stop();
                stopRecording();
            }
        });

        QObject::connect(record_timer, &MyTimer::timeout, [&writer, &image](){
            writer.write(image);
        });

        QObject::connect(btnCapture, &QPushButton::clicked, [&image](){
            // 1. 이미지 데이터가 비어있으면 캡처하지 않음 (예외 처리)
            if (image.empty()) {
                qDebug() << "Image is empty!";
                return;
            }

            // 2. 현재 날짜/시간 기반 파일명 생성 (filename = now + ".png")
            QString now = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
            std::string filename = now.toStdString() + ".png";

            // 3. 화면 표시용 RGB 이미지를 저장용 BGR로 변환 후 이미지 파일 작성
            // cv2.imwrite(filename, cv2.cvtColor(self.image, cv2.COLOR_RGB2BGR)) 에 대응
            cv::Mat saveImage;
            cv::cvtColor(image, saveImage, cv::COLOR_RGB2BGR);
            if (cv::imwrite(filename, saveImage)) {
                qDebug() << "Image saved:" << QString::fromStdString(filename);
            }
            else {
                qDebug() << "Image save failed!";
            }
        });
        
        myWindow->show();
    }

    return app.exec();
}