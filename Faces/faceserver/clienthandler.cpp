#include "clienthandler.h"
#include "connectionpool.h"
#include <opencv2/core.hpp>

ClientHandler::ClientHandler(const QByteArray& data, const QHostAddress& senderAddress,
                             quint16 senderPort, QObject* parent)
    : QObject(parent), data(data), senderAddress(senderAddress), senderPort(senderPort) {
    db = ConnectionPool::getInstance().getConnection();
    query = QSqlQuery(db);
}


ClientHandler::~ClientHandler() {
    ConnectionPool::getInstance().releaseConnection(db);
}


void ClientHandler::handleData() {
    qDebug()<<"收到数据";
    QString dataStr = QString::fromUtf8(data);
    QStringList parts = dataStr.split(", Features: ");
    if (parts.size() != 2) {
        return;
    }
    QString location = parts[0].mid(QString("Location: ").length()).trimmed();
    QString features = parts[1].trimmed();
    qDebug()<<"数据库开始查询";
    query.prepare("SELECT name, face_features FROM FaceInfo WHERE location = ?");
    query.addBindValue(location);
    qDebug()<<"数据库查询完了,开始遍历结果";
    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(0).toString();
            QString dbFeatures = query.value(1).toString();
            if (compareFeatures(dbFeatures, features)) {
                insertAttendanceRecord(name, location);
                break;
            }
        }
    }
    qDebug()<<"遍历完了";
    this->~ClientHandler();
}


bool ClientHandler::compareFeatures(const QString& dbFeatures, const QString& receivedFeatures)
{
    QStringList dbValues = dbFeatures.split(';', Qt::SkipEmptyParts);
    std::vector<cv::Point2f> dbPoints;
    for (const QString& value : dbValues) {
        QStringList point = value.split(',');
        if (point.size() == 2) {
            dbPoints.emplace_back(point[0].toFloat(), point[1].toFloat());
        }
    }
    QStringList receivedValues = receivedFeatures.split(';', Qt::SkipEmptyParts);
    std::vector<cv::Point2f> receivedPoints;
    for (const QString& value : receivedValues) {
        QStringList point = value.split(',');
        if (point.size() == 2) {
            receivedPoints.emplace_back(point[0].toFloat(), point[1].toFloat());
        }
    }
    cv::Mat dbMat = cv::Mat(dbPoints).reshape(1, 2 * dbPoints.size()).t();
    cv::Mat receivedMat = cv::Mat(receivedPoints).reshape(1, 2 * receivedPoints.size()).t();
    cv::Mat diff;
    cv::absdiff(dbMat, receivedMat, diff);
    cv::multiply(diff, diff, diff);
    float distance = static_cast<float>(cv::sqrt(cv::sum(diff)[0]));
    const float threshold = 25.0f;
    if(distance<threshold){
        qDebug()<<threshold-distance;
    }
    return distance < threshold;
}


void ClientHandler::insertAttendanceRecord(const QString& name, const QString& location)
{
    QUdpSocket udpSocket;
    QByteArray responseData = name.toUtf8();
    udpSocket.writeDatagram(responseData, QHostAddress(senderAddress), 54321);
    qDebug()<<"--------------------认证成功--------------------"<<name;
    query.prepare("INSERT INTO AttendanceRecords (name, location) VALUES (?, ?)");
    query.addBindValue(name);
    query.addBindValue(location);
    query.exec();
}
