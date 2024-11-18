#include "connectionpool.h"
#include <QDebug>

ConnectionPool& ConnectionPool::getInstance()//获取单例实例
{
    static ConnectionPool instance;
    return instance;
}


QSqlDatabase ConnectionPool::getConnection() // 获取数据库连接
{
    QMutexLocker locker(&mutex);
    if (!pool.isEmpty()) {
        QSqlDatabase db = pool.dequeue();
        if (db.isOpen()) {
            qDebug() << "有现成的连接";
            return db;
        } else {
            db.close();
            qDebug() << "连接已关闭，丢弃该连接，尝试创建新连接...";
            return getConnection();
        }
    }
    //如果没有可用的连接且池未满，则新建连接
    if (pool.size() < maxConnections) {
        QString connectionName = QString("Connection_%1").arg(connectionCounter++);
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setDatabaseName(dbName);
        db.setUserName("root");
        db.setPassword("123456");
        if (!db.open()) {
            qDebug() << "数据库打开失败:" << db.lastError().text();
            return QSqlDatabase();
        }
        return db; // 返回新连接
    } else {
        qDebug() << "数据库最大连接数已达到!";
        return QSqlDatabase();
    }
}


void ConnectionPool::releaseConnection(QSqlDatabase db)//释放连接
{
    QMutexLocker locker(&mutex);
    if (pool.size() < maxConnections) {
        if (db.isOpen()) {
            qDebug()<<"放回池了";
            pool.enqueue(db);
        } else {
            db.close();
        }
    } else {
        db.close();
    }
}


void ConnectionPool::setMaxConnections(int max)//设置最大连接数
{
    QMutexLocker locker(&mutex);
    maxConnections = max;
}


int ConnectionPool::getMaxConnections() const//获取当前最大连接数
{
    return maxConnections;
}


ConnectionPool::ConnectionPool() : maxConnections(50)//私有构造函数  默认最大连接数
{
    //建立数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", dbName);
    db.setDatabaseName(dbName);
    db.setUserName("root");
    db.setPassword("123456");
    if (!db.open()) {
        qDebug() << "打开数据库失败" << db.lastError().text();
    }
    else{
        QSqlQuery query(db);
        //创建考勤记录表(自增长id，姓名，地点，时间)
        query.exec("CREATE TABLE IF NOT EXISTS AttendanceRecords ("
                   "name VARCHAR(255),"
                   "location VARCHAR(255),"
                   "time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                   "PRIMARY KEY (name, location, time)"
                   ");");
        if (query.lastError().isValid()) {
            qDebug() << "创建考勤记录表失败:" << query.lastError().text();
        }
        // 创建人脸信息表(自增长id，姓名，地点，人脸特征)
        query.exec("CREATE TABLE IF NOT EXISTS FaceInfo ("
                   "id INT AUTO_INCREMENT PRIMARY KEY,"
                   "name VARCHAR(255),"
                   "location VARCHAR(255),"
                   "face_features LONGTEXT);");
        if (query.lastError().isValid()) {
            qDebug() << "创建人脸信息表失败:" << query.lastError().text();
        } else {
            query.exec("CREATE INDEX idx_location ON FaceInfo (location);");
            if (query.lastError().isValid()) {
                qDebug() << "创建索引失败:" << query.lastError().text();
            }
        }
        //创建设备信息表(设备id，地点)
        query.exec("CREATE TABLE IF NOT EXISTS DeviceInfo ("
                   "id INT AUTO_INCREMENT PRIMARY KEY,"
                   "device_id VARCHAR(255),"
                   "location VARCHAR(255));");
        if (query.lastError().isValid()) {
            qDebug() << "创建设备信息表失败:" << query.lastError().text();
        }
    }
}


ConnectionPool::~ConnectionPool()//私有析构函数
{
    while (!pool.isEmpty()) {
        QSqlDatabase db = pool.dequeue();
        db.close();//关闭所有连接
    }
}


