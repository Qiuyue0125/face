#include "checkfacethread.h"
#include <opencv2/imgcodecs.hpp>
#include <dlib/opencv.h>
#include <QDebug>
#include <exception>

CheckFaceThread::CheckFaceThread(cv::CascadeClassifier faceCascade, std::vector<std::pair<int, int>> &landmark, const QUrl &url, dlib::frontal_face_detector &detector,
                                 dlib::shape_predictor &predictor, QObject *parent)
    : QThread(parent), faceCascade(faceCascade), landmarks(landmark), imageUrl(url), faceDetector(detector), shapePredictor(predictor) {}


void CheckFaceThread::run()
{
    cv::Mat image = cv::imread(imageUrl.toLocalFile().toStdString());
    if (image.empty()) {
        emit detectionFailed();
        return;
    }
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayImage, faces, 1.1, 2,
                                 cv::CASCADE_FIND_BIGGEST_OBJECT | cv::CASCADE_DO_CANNY_PRUNING,
                                 cv::Size(120, 120));

    if (faces.size() != 1) {
        emit detectionFailed();
        return;
    }
    cv::Rect faceRect = faces[0];
    cv::Mat faceImage = grayImage(faceRect).clone();
    cv::Size standardSize(150, 150);
    cv::resize(faceImage, faceImage, standardSize);
    dlib::cv_image<unsigned char> dlibImage(faceImage);
    dlib::rectangle dlibFace(0, 0, standardSize.width, standardSize.height);
    dlib::full_object_detection shape = shapePredictor(dlibImage, dlibFace);
    cv::Point2f eyesCenter((shape.part(36).x() + shape.part(45).x()) / 2.0,
                           (shape.part(36).y() + shape.part(45).y()) / 2.0);
    double dx = shape.part(45).x() - shape.part(36).x();
    double dy = shape.part(45).y() - shape.part(36).y();
    double angle = atan2(dy, dx) * 180.0 / CV_PI;
    cv::Mat rotMat = cv::getRotationMatrix2D(eyesCenter, angle, 1.0);
    cv::Mat alignedFace;
    cv::warpAffine(faceImage, alignedFace, rotMat, faceImage.size());
    landmarks.clear();
    for (int i = 0; i < shape.num_parts(); ++i) {
        landmarks.emplace_back(shape.part(i).x(), shape.part(i).y());
    }
    emit detectionSucceeded();
}
