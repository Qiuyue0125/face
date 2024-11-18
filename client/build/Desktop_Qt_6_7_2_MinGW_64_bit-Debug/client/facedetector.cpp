#include "facedetector.h"
#include <qdebug.h>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QThreadPool>
#include <opencv2/opencv.hpp>
#include <QMutexLocker>
#include <QTimer>

FaceDetector::FaceDetector(QObject *parent) : QObject(parent)
{
    cap.open(0);
    if (!cap.isOpened())
    {
        qDebug() << "无法打开摄像头";
    }
    if (!faceCascade.load("D:/OpenCV/build/etc/haarcascades/haarcascade_frontalface_alt.xml"))
    {
        qDebug() << "无法加载人脸检测级联分类器";
    }
    timer = new QTimer(this);
    imageProvider = ImageProvider::instance();
    connect(timer, &QTimer::timeout, this, &FaceDetector::processFrame);
}


void FaceDetector::startDetection(const QVariant &addr)
{
    address = addr.toString();
    isDetecting = true;
    if (!cap.isOpened()){
        cap.open(0);
    }
    timer->start(33);
    qDebug() <<"开始捕获,检测仪地址是"<<address;
}


void FaceDetector::processFrame()
{
    if (!isDetecting) return;
    Mat image;
    cap >> image;
    if (image.empty()) return;
    int currentCount;
    {
        QMutexLocker locker(&mutex);
        currentCount = count++;
    }
    DealImage* task = new DealImage(image.clone(), currentCount, faceCascade, this);
    connect(task, &DealImage::done, this, &FaceDetector::getResult);
    task->start();
}


void FaceDetector::detectQuit()
{
    isDetecting = false;
    timer->stop();
    imageProvider->images.clear();
    currentIndex = 0;
    count = 0;
    cap.release();
}


void FaceDetector::getResult(const QImage ima, const int count)
{
    QMutexLocker locker(&mutex);
    int ct = count % 500;
    QString id = QString::number(ct);
    imageProvider->addImage(id, ima);
    while(imageProvider->images.contains(QString::number(currentIndex))){
        emit imageCaptured();
        currentIndex++;
        if(currentIndex == 500){
            imageProvider->images.clear();
        }
        currentIndex %= 500;
    }
}


DealImage::DealImage(const Mat& image, int count, const CascadeClassifier &faceCascade, QObject* parent)
    : QThread(parent), myImage(image), myCount(count), myFaceCascade(faceCascade) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
}


void DealImage::run()
{
    flip(myImage, myImage, 1);
    Mat face;
    face = myImage(Range(0, myImage.rows / 3 * 2), Range(myImage.cols / 6, myImage.cols / 6 * 5));
    std::vector<Rect> faces;
    if (myCount % 20 == 0) {
        myFaceCascade.detectMultiScale(face, faces, 1.1, 2, CASCADE_FIND_BIGGEST_OBJECT|CASCADE_DO_CANNY_PRUNING, Size(120,120));
        if(faces.size() == 1){
            qDebug()<<"有一个人脸";
        }
    }
    emit done(MatToQImage(myImage), myCount);
}


QImage DealImage::MatToQImage(const cv::Mat& mat)
{
    switch (mat.type()) {
    case CV_8UC1:{
        QImage img((uchar*)mat.data,mat.cols,mat.rows,mat.cols*1,QImage::Format_Grayscale8);
        return img;
    }
    case CV_8UC3:{
        QImage img((uchar*)mat.data,mat.cols,mat.rows,mat.cols*3,QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    case CV_8UC4:{
        QImage img((uchar*)mat.data,mat.cols,mat.rows,mat.cols*4,QImage::Format_ARGB32);
        return img;
    }
    default:{
        QImage img;
        return img;
    }
    }
}
