#ifndef SERVERCPP_H
#define SERVERCPP_H

#include <QObject>
#include <QSqlTableModel>
#include <QUdpSocket>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <opencv2/opencv.hpp>
#include "connectionpool.h"

class CustomAttendanceModel;

class ServerCPP : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CustomAttendanceModel* attendanceModel READ getAttendanceModel CONSTANT)
    Q_PROPERTY(QSqlTableModel* deviceModel READ getDeviceModel CONSTANT)

public:
    explicit ServerCPP(QObject *parent = nullptr);
    ~ServerCPP();
    Q_INVOKABLE void listen();
    Q_INVOKABLE void stopListen();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void addDevice(const QVariant &device_id, const QVariant &location);
    Q_INVOKABLE void chooseFaceImage(const QUrl &url);
    Q_INVOKABLE void addFace(const QVariant name, const QVariant location);
    Q_INVOKABLE void sql(const QVariant &sql);
    CustomAttendanceModel* getAttendanceModel() const { return attendanceModel; }
    QSqlTableModel* getDeviceModel() const { return deviceModel; }

signals:
    void succeed();
    void fail();
    void logoSucceed();
    void logoFail();

private slots:
    void processPendingDatagrams();

private:
    ConnectionPool* pool;
    CustomAttendanceModel *attendanceModel;
    QSqlTableModel *deviceModel;
    QUdpSocket *udpSocket;
    quint16 port = 12345;
    cv::CascadeClassifier faceCascade;
    std::vector<float> features;
    std::vector<std::pair<int, int>> landmarks;
    dlib::frontal_face_detector detector;
    dlib::shape_predictor predictor;
    QSqlDatabase db;
};


class CustomAttendanceModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit CustomAttendanceModel(QObject *parent = nullptr)
        : QSqlQueryModel(parent)
    {}
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};



#endif // SERVERCPP_H
