#include "facedetector.h"
#include <qdebug.h>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QThreadPool>
#include <QMutexLocker>
#include <QTimer>

FaceDetector::FaceDetector(QObject *parent) : QObject(parent)
{
    cap.open(0);
    if (!cap.isOpened())
    {
        qDebug() << "无法打开摄像头";
    }
    imageProvider = ImageProvider::instance();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FaceDetector::processFrame);
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, localPort);
    connect(udpSocket, &QUdpSocket::readyRead, this, &FaceDetector::dealDatas);
}


FaceDetector::~FaceDetector(){
    if(timer){
        timer->deleteLater();
    }
    if (cap.isOpened())
    {
        cap.release();
    }
    if(udpSocket){
        udpSocket->deleteLater();
    }
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
        count %= 500;
        currentCount = count++;
    }
    DealImage* task = new DealImage(address, image.clone(), currentCount);
    connect(task, &QThread::finished, task, &QObject::deleteLater);
    connect(task, &DealImage::done, this, &FaceDetector::getResult);
    task->start();
}


void FaceDetector::dealDatas()
{
    QByteArray buffer;
    buffer.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(buffer.data(), buffer.size());
    QString name = QString::fromUtf8(buffer);
    emit identitySuccess(name);
}


void FaceDetector::startDetection(const QVariant &addr)
{
    address = addr.toString();
    isDetecting = true;
    if (!cap.isOpened()){
        cap.open(0);
    }
    timer->start(33);
    qDebug() <<"开始捕获,本地地址是"<<address;
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


