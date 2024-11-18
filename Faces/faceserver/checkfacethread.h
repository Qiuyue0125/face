#ifndef CHECKFACETHREAD_H
#define CHECKFACETHREAD_H

#include <QObject>
#include <QThread>
#include <QUrl>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>

class CheckFaceThread : public QThread {
    Q_OBJECT

public:
    explicit CheckFaceThread(cv::CascadeClassifier faceCascade, std::vector<std::pair<int, int>> &landmark, const QUrl &url, dlib::frontal_face_detector &detector,
                             dlib::shape_predictor &predictor, QObject *parent = nullptr);

signals:
    void detectionFailed();
    void detectionSucceeded();

protected:
    void run() override;

private:
    QUrl imageUrl;
    std::vector<std::pair<int, int>> &landmarks;
    dlib::frontal_face_detector &faceDetector;
    dlib::shape_predictor &shapePredictor;
    cv::CascadeClassifier &faceCascade;
};

#endif // CHECKFACETHREAD_H
