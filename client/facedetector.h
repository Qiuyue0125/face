#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QUrl>
#include <QWaitCondition>
#include <QMutex>
#include <QHash>
#include <QUdpSocket>
#include "imageprovider.h"
#include "threads.h"

using namespace cv;

class FaceDetector : public QObject {
    Q_OBJECT

public:
    explicit FaceDetector(QObject *parent = nullptr);
    ~FaceDetector();
    void processFrame();
    void dealDatas();
    Q_INVOKABLE void startDetection(const QVariant &addr);
    Q_INVOKABLE void detectQuit();

signals:
    void imageCaptured();
    void identitySuccess(const QVariant &name);

public slots:
    void getResult(const QImage ima, const int count);

private:
    QMutex mutex;
    QWaitCondition condition;
    int currentIndex = 0;
    int count = 0;
    bool isDetecting = true;
    cv::VideoCapture cap;
    QTimer *timer;
    ImageProvider *imageProvider = nullptr;
    QString address;//本地物理地址
    quint16 localPort = 54321;//本地监听端口
    QUdpSocket* udpSocket;
    bool udpFlag = true;
};

#endif // FACEDETECTOR_H
