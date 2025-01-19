#include "servercpp.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QNetworkDatagram>
#include <QThreadPool>
#include <clienthandler.h>
#include "checkfacethread.h"

ServerCPP::ServerCPP(QObject *parent)
    : QObject{parent}
{
    pool = &ConnectionPool::getInstance();
    db = pool->getConnection();
    attendanceModel = new CustomAttendanceModel(this);
    attendanceModel->setQuery("SELECT * FROM AttendanceRecords", db);
    deviceModel = new QSqlTableModel(this, db);
    deviceModel->setTable("DeviceInfo");
    deviceModel->select();
    if (!faceCascade.load("D:/OpenCV/build/etc/haarcascades/haarcascade_frontalface_alt.xml"))
    {
        qDebug() << "无法加载人脸检测级联分类器";
    }
    detector = dlib::get_frontal_face_detector();
    try {
        dlib::deserialize("D:/dlib/dat/shape_predictor_68_face_landmarks.dat") >> predictor;
        qDebug() << "Shape predictor 加载成功.";
    } catch (const dlib::serialization_error& e) {
        qDebug() << "Shape predictor 加载失败 " << e.what();
    }
    udpSocket = new QUdpSocket(this);
    //连接信号槽
    connect(udpSocket, &QUdpSocket::readyRead, this, &ServerCPP::processPendingDatagrams);
}


ServerCPP::~ServerCPP()
{
    pool->releaseConnection(db);
}


void ServerCPP::listen()
{
    udpSocket->bind(QHostAddress::Any, port);
}


void ServerCPP::stopListen()
{
    udpSocket->close();
}


void ServerCPP::refresh()
{
    deviceModel->select();
    attendanceModel->setQuery("SELECT * FROM AttendanceRecords", db);
    qDebug()<<"模型已刷新";
}


void ServerCPP::addDevice(const QVariant &device_id, const QVariant &location)
{
    QSqlQuery qry(db);
    qry.prepare("INSERT INTO DeviceInfo (device_id, location) VALUES (:device_id, :location)");
    qry.bindValue(":device_id", device_id);
    qry.bindValue(":location", location);
    if(qry.exec()){
        emit succeed();
    }
    else{
        emit fail();
    }
    refresh();
}


void ServerCPP::chooseFaceImage(const QUrl &url)
{
    if (url.isEmpty()) {
        emit logoFail();
        return;
    }
    CheckFaceThread *thread = new CheckFaceThread(faceCascade, landmarks, url, detector, predictor);
    connect(thread, &CheckFaceThread::detectionSucceeded, this, [this, thread](){
        emit logoSucceed();
        thread->deleteLater();
    });
    connect(thread, &CheckFaceThread::detectionFailed, this, [this, thread]() {
        emit logoFail();
        thread->deleteLater();
    });
    connect(thread, &CheckFaceThread::finished,[&thread]() {
        delete thread;
    });
    thread->start();
}


void ServerCPP::addFace(const QVariant name, const QVariant location)
{
    if (landmarks.empty()){
        emit fail();
        return;
    }
    QStringList landmarkList;
    for (const auto &pair : landmarks) {
        landmarkList << QString::number(pair.first) + "," + QString::number(pair.second);
    }
    QString landmarkStr = landmarkList.join(";");
    QSqlQuery qry(db);
    qry.prepare("INSERT INTO FaceInfo (name, location, face_features) VALUES (:name, :location, :face_features)");
    qry.bindValue(":name", name.toString());
    qry.bindValue(":location", location.toString());
    qry.bindValue(":face_features", landmarkStr);
    if(qry.exec()){
        emit succeed();
    } else {
        emit fail();
    }
}


void ServerCPP::sql(const QVariant &sql)
{
    QSqlQuery qry(db);
    qry.prepare(sql.toString());
    if(qry.exec()){
        emit succeed();
    }
    else{
        emit fail();
    }
    refresh();
}


void ServerCPP::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray data = datagram.data();
        QHostAddress senderAddress = datagram.senderAddress();
        quint16 senderPort = datagram.senderPort();
        ClientHandler* handler = new ClientHandler(data, senderAddress, senderPort, this);
        QThreadPool::globalInstance()->start([handler]() {
            connect(handler, &ClientHandler::done, [handler]() {
                handler->deleteLater();
            });
            handler->handleData();
        });
    }
}


QVariant CustomAttendanceModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && index.column() == 2) {
        QVariant timeData = QSqlQueryModel::data(index, role);
        QDateTime dateTime = QDateTime::fromString(timeData.toString(), Qt::ISODate);
        return dateTime.toString("yyyy.MM.dd hh:mm:ss");
    }
    return QSqlQueryModel::data(index, role);
}
