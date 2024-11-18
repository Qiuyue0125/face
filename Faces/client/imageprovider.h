#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H
#include <QQuickImageProvider>

class ImageProvider : public QQuickImageProvider {

public:
    static ImageProvider* instance();
    ImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    void addImage(const QString &id, const QImage &image);

public:
    QHash<QString, QImage> images;
};
#endif // IMAGEPROVIDER_H
