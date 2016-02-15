#ifndef DIFFIMAGE_H
#define DIFFIMAGE_H

#include <QObject>
#include <QImage>

struct Header
{
    int width;
    int height;
    double negativeCoef;
    double positiveCoef;
};


class DiffImage : public QObject
{
    Q_OBJECT
public:

    enum DiffQuality {WORST = 1, SUPER_BAD = 2, VERY_BAD = 4, BAD = 8, NOT_GOOD = 16, NORMAL = 32, GOOD = 64, VERY_GOOD = 127};

    explicit DiffImage(QObject *parent = 0);
    static QImage createQuantDeltaEncoded(QImage &base, QImage &out, int quality);
    static QImage recoverQuantDeltaEncoded(QImage &base, int quality);
    static QImage recoverLinearDeltaEncoded(QImage &base);
    static QImage createLinearDeltaEncoded(QImage &base, QImage & out, int quality);
    static QImage buildFromDeltaEncoded(QImage &delta);
    static uchar* produceData(int w, int h);



signals:

public slots:
private:
};

#endif // DIFFIMAGE_H
