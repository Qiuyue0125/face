#include "imageprovider.h"

 ImageProvider* ImageProvider::instance() {
    static ImageProvider* inst = new ImageProvider();
    return inst;
}


QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)  {
    if(id != "-1"){
        return images.value(id);}
    return QImage(":/resources/Back.png");
}


void ImageProvider::addImage(const QString &id, const QImage &image) {
    images.insert(id, image);
}
