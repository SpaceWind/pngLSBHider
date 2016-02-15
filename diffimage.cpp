#include "diffimage.h"
#include "imageoperations.h"

DiffImage::DiffImage(QObject *parent) : QObject(parent)
{

}

QImage DiffImage::createQuantDeltaEncoded(QImage &base, QImage &out, int quality)
{
    uchar * emptyData = produceData(base.width(), base.height());
    QImage result(emptyData, base.width(), base.height(), base.width() * 4, QImage::Format_ARGB32);
    out.fill(0);
    for (int x = 0; x < base.height(); x++)
    {
        QRgb* baseLine = (QRgb*)base.scanLine(x);
        QRgb* resultLine = (QRgb*)result.scanLine(x);
        QRgb* outLine = (QRgb*)out.scanLine(x);
        for (int y = 0; y < base.width(); y++)
        {
            if (y == 0)
            {
                resultLine[0] = qRgb(qRed(baseLine[0]),qGreen(baseLine[0]), qBlue(baseLine[0]));
                outLine[0] = qRgb(qRed(baseLine[0]),qGreen(baseLine[0]), qBlue(baseLine[0]));
                continue;
            }
            int rDiff, gDiff, bDiff;
            rDiff = qRed(baseLine[y]) - qRed(outLine[y-1]);
            gDiff = qGreen(baseLine[y]) - qGreen(outLine[y-1]);
            bDiff = qBlue(baseLine[y]) - qBlue(outLine[y-1]);

            rDiff/=quality;
            gDiff/=quality;
            bDiff/=quality;

          //  rDiff = std::min(std::max(rDiff,-quality),quality+1);
          //  gDiff = std::min(std::max(gDiff,-quality),quality+1);
          //  bDiff = std::min(std::max(bDiff,-quality),quality+1);

            resultLine[y] = qRgb(rDiff+127,gDiff+127,bDiff+127);
            outLine[y] = qRgb(qRed(outLine[y-1]) + rDiff*quality, qGreen(outLine[y-1]) + gDiff*quality, qBlue(outLine[y-1]) + bDiff*quality);
        }
    }
    return result;
}

QImage DiffImage::recoverQuantDeltaEncoded(QImage &base, int quality)
{
    uchar * emptyData = produceData(base.width(), base.height());
    QImage result(emptyData, base.width(), base.height(), base.width() * 4, QImage::Format_ARGB32);

    for (int x = 0; x < base.height(); x++)
    {
        QRgb* baseLine = (QRgb*)base.scanLine(x);
        QRgb* resultLine = (QRgb*)result.scanLine(x);
        for (int y = 0; y < base.width(); y++)
        {
            if (y == 0)
            {
                resultLine[0] = qRgb(qRed(baseLine[0]),qGreen(baseLine[0]), qBlue(baseLine[0]));
                continue;
            }
            int rDiff, gDiff, bDiff;
            rDiff = (qRed(baseLine[y]) - 127) * quality;
            gDiff = (qGreen(baseLine[y]) - 127) * quality;
            bDiff = (qBlue(baseLine[y]) - 127) * quality;

            resultLine[y] = qRgb(rDiff+127,gDiff+127,bDiff+127);
            resultLine[y] = qRgb(kClamp(qRed(resultLine[y-1]) + rDiff,0,255),
                                 kClamp(qGreen(resultLine[y-1]) + gDiff,0,255),
                                 kClamp(qBlue(resultLine[y-1]) + bDiff,0,255));
        }
    }
    return result;
}

QImage DiffImage::recoverLinearDeltaEncoded(QImage &base)
{
    uchar * emptyData = produceData(base.width(), base.height());
    QImage result(emptyData, base.width(), base.height(), base.width() * 4, QImage::Format_ARGB32);

    for (int x = 0; x < base.height(); x++)
    {
        QRgb* baseLine = (QRgb*)base.scanLine(x);
        QRgb* resultLine = (QRgb*)result.scanLine(x);
        for (int y = 0; y < base.width(); y++)
        {
            if (y == 0 || y == 1)
            {
                resultLine[0] = qRgb(qRed(baseLine[0]),qGreen(baseLine[0]), qBlue(baseLine[0]));
                continue;
            }
            int rDiff, gDiff, bDiff;
            rDiff = qRed(baseLine[y]) - 127;
            gDiff = qGreen(baseLine[y]) - 127;
            bDiff = qBlue(baseLine[y]) - 127;

          //  resultLine[y] = qRgb(rDiff+127,gDiff+127,bDiff+127);
            resultLine[y] = qRgb(qRed(resultLine[y-1]) + rDiff, qGreen(resultLine[y-1]) + gDiff, qBlue(resultLine[y-1]) + bDiff);
        }
    }
    return result;
}

QImage DiffImage::createLinearDeltaEncoded(QImage &base, QImage &out, int quality)
{
    uchar * emptyData = produceData(base.width(), base.height());
    QImage result(emptyData, base.width(), base.height(), base.width() * 4, QImage::Format_ARGB32);
    out.fill(0);
    for (int x = 0; x < base.height(); x++)
    {
        QRgb* baseLine = (QRgb*)base.scanLine(x);
        QRgb* resultLine = (QRgb*)result.scanLine(x);
        QRgb* outLine = (QRgb*)out.scanLine(x);
        for (int y = 0; y < base.width(); y++)
        {
            if (y == 0 || y == 1)
            {
                resultLine[0] = qRgb(qRed(baseLine[0]),qGreen(baseLine[0]), qBlue(baseLine[0]));
                outLine[0] = qRgb(qRed(baseLine[0]),qGreen(baseLine[0]), qBlue(baseLine[0]));
                continue;
            }
            int rDiff, gDiff, bDiff;
            rDiff = qRed(baseLine[y]) - qRed(outLine[y-1]);
            gDiff = qGreen(baseLine[y]) - qGreen(outLine[y-1]);
            bDiff = qBlue(baseLine[y]) - qBlue(outLine[y-1]);

            rDiff = std::min(std::max(rDiff,-quality),quality+1);
            gDiff = std::min(std::max(gDiff,-quality),quality+1);
            bDiff = std::min(std::max(bDiff,-quality),quality+1);

            resultLine[y] = qRgb(rDiff+127,gDiff+127,bDiff+127);
            outLine[y] = qRgb(qRed(outLine[y-1]) + rDiff, qGreen(outLine[y-1]) + gDiff, qBlue(outLine[y-1]) + bDiff);
        }
    }
    return result;
}

uchar * DiffImage::produceData(int w, int h)
{
    uchar *r, *tmp;
    r = new uchar[w * 4 * h];
    for (int i = 0; i < h; ++i)
    {
        tmp = r + (i * w * 4); // stride
        for (int j = 0; j < w * 4; j += 4)
        {
            tmp[j] = 0;
            tmp[j + 1] = 0;
            tmp[j + 2] = 0;
            tmp[j + 3] = 0;
        }
    }
    return r;
}













