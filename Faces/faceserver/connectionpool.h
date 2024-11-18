#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "qmutex.h"
#include <QSqlDatabase>
#include <QObject>
#include <QQuickItem>
#include <QSharedDataPointer>
#include <QQueue>
#include <QSqlError>
#include <QSqlQuery>


class ConnectionPool {//连接池
public:
    static ConnectionPool& getInstance();//获取单例实例
    QSqlDatabase getConnection();//获取数据库连接
    void releaseConnection(QSqlDatabase db);//释放连接
    void setMaxConnections(int max);//设置最大连接数
    int getMaxConnections() const;//获取当前最大连接数

private:
    ConnectionPool();//私有构造函数
    ~ConnectionPool();//私有析构函数
    QQueue<QSqlDatabase> pool;//连接池
    QMutex mutex;//互斥锁
    int maxConnections;//最大连接数
    QString dbName = "faceserver";//数据库名字
    int connectionCounter = 0;//连接个数 起名字
};

#endif // CONNECTIONPOOL_H
