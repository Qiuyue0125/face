#include "threads.h"
#include <QStandardPaths>
#include <QImage>
#include <QMutex>
#include <QByteArray>
#include <QMutexLocker>

DealImage::DealImage(const QString& locat, const Mat& image, int count,  QObject* parent)
    : QThread(parent), location(locat), myImage(image), myCount(count){
    QString dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
}


void DealImage::run()
{
    flip(myImage, myImage, 1);
    Mat face;
    face = myImage(Range(0, myImage.rows / 3 * 2), Range(myImage.cols / 6, myImage.cols / 6 * 5));
    std::vector<Rect> faces;
    if (myCount % 6 == 0) {
        FaceAnalysisThread* analysisThread = new FaceAnalysisThread(location, face);
        connect(analysisThread, &QThread::finished, analysisThread, &QObject::deleteLater);
        analysisThread->start();
    }
    emit done(MatToQImage(myImage), myCount);
    this->~DealImage();
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


bool FaceAnalysisThread::initialized = false;
dlib::frontal_face_detector FaceAnalysisThread::detector;
dlib::shape_predictor FaceAnalysisThread::shapePredictor;
CascadeClassifier FaceAnalysisThread::faceCascade;


FaceAnalysisThread::FaceAnalysisThread(const QString& locat, const Mat &image, QObject *parent)
    : QThread(parent), location(locat), image(image.clone()){
    if(!initialized){
        init();
        initialized = true;
    }
}


void FaceAnalysisThread::run()
{
    if (image.empty()) {
        return;
    }
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayImage, faces, 1.1, 2,
                                 cv::CASCADE_FIND_BIGGEST_OBJECT | cv::CASCADE_DO_CANNY_PRUNING,
                                 cv::Size(120, 120));

    if (faces.size() != 1) {
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
    std::vector<std::pair<int, int>> landmarks;
    for (int i = 0; i < shape.num_parts(); ++i) {
        landmarks.emplace_back(shape.part(i).x(), shape.part(i).y());
    }
    QStringList landmarkList;
    for (const auto &pair : landmarks) {
        landmarkList << QString::number(pair.first) + "," + QString::number(pair.second);
    }
    QString landmarkStr = landmarkList.join(";");
    QByteArray message = QString("Location: %1, Features: %2").arg(location).arg(landmarkStr).toUtf8();
    QUdpSocket *udpSocket = new QUdpSocket();
    udpSocket->writeDatagram(message, targetAddress, targetPort);
    udpSocket->deleteLater();
}


QMutex mutex;
void FaceAnalysisThread::init() {
    QMutexLocker locker(&mutex);
    if (!initialized) {
        detector = dlib::get_frontal_face_detector();
        try {
            dlib::deserialize("D:/dlib/dat/shape_predictor_68_face_landmarks.dat") >> shapePredictor;
            if (!faceCascade.load("D:/OpenCV/build/etc/haarcascades/haarcascade_frontalface_alt.xml")) {
                throw std::runtime_error("Failed to load cascade classifier");
            }
            initialized = true;
        } catch (const std::exception& e) {
            qDebug() << "upload fail" << e.what();
        }
    }
}
