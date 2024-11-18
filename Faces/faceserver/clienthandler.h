#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <Qdebug>
#include <Qobject>
#include <Qudpsocket>
#include <QSqldatabase>
#include <QSqlQuery>

class ClientHandler : public QObject {
    Q_OBJECT

public:
    ClientHandler(const QByteArray& data, const QHostAddress& senderAddress,
                  quint16 senderPort, QObject* parent = nullptr);
    ~ClientHandler();
    void handleData();
    bool compareFeatures(const QString& dbFeatures, const QString& receivedFeatures);
    void insertAttendanceRecord(const QString& name, const QString& location);

private:
    QByteArray data;
    QHostAddress senderAddress;
    quint16 senderPort;
    QSqlDatabase db;
    QSqlQuery query;
};

#endif // CLIENTHANDLER_H
