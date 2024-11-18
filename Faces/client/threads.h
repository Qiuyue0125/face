#ifndef THREADS_H
#define THREADS_H

#include <QThread>
#include <QRunnable>
#include <QUdpSocket>
#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>

using namespace cv;

class DealImage : public QThread {
    Q_OBJECT

public:
    explicit DealImage(const QString& locat, const Mat& image, int count, QObject* parent = nullptr);
    void run() override;
    QImage MatToQImage(const Mat& mat);

signals:
    void done(const QImage ima, const int count);

private:
    Mat myImage;
    int myCount;
    QString location;
    CascadeClassifier faceCascade;
};


class FaceAnalysisThread : public QThread {
    Q_OBJECT

public:
    explicit FaceAnalysisThread(const QString& locat, const Mat &image, QObject *parent = nullptr);
    void run() override;
    static void init();

private:
    Mat image;
    QString location;
    QHostAddress targetAddress = QHostAddress("127.0.0.1");//server地址
    quint16 targetPort = 12345;//server端口
    static bool initialized;
    static dlib::frontal_face_detector detector;
    static dlib::shape_predictor shapePredictor;
    static CascadeClassifier faceCascade;
};

#endif // THREADS_H
