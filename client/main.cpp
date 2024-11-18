#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include "facedetector.h"
#include "imageprovider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<FaceDetector>("client", 1, 0, "FaceDetector");
    QQmlApplicationEngine engine;
    ImageProvider *provider = ImageProvider::instance();
    engine.addImageProvider(QLatin1String("provider"), provider);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    app.setWindowIcon(QIcon(":/resources/suliao_avator.png"));
    engine.loadFromModule("client", "Main");
    return app.exec();
}
