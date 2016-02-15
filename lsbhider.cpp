#include "lsbhider.h"

ImageLayerCursor::ImageLayerCursor(QImage *img)
{
    image = img;
    left = 0;
    top = 0;
    color = 0;
    bit = 0;
    pos = 0;
    width = img->width();
    height = img->height();
    lines = new QRgb*[img->height()];
    for (int i=0; i<img->height(); i++)
        lines[i] = (QRgb*)img->scanLine(i);
}

ImageLayerCursor::~ImageLayerCursor()
{
    delete lines;
}

bool ImageLayerCursor::next()
{
    int prev = color;
    pos++;
    color = (color +1) % 3;
    if (color < prev)
    {
        prev = left;
        left = (left + 1) % width;
        if (left < prev)
        {
            prev = top;
            top = (top + 1) % height;
            if (top < prev)
            {
                prev = bit;
                bit = (bit + 1) % 8;
                if (bit < prev)
                {
                    reset();
                    return false;
                }
            }
        }
    }
    return true;
}

uchar ImageLayerCursor::readByte()
{
    uchar result = 0;
    for (int i=0; i<8; i++)
    {
        bool resultBit;
        switch (color)
        {
        default:
        case 0:
            resultBit = (qRed(lines[top][left]) & (1 << bit)) > 0 ? true : false;
            break;
        case 1:
            resultBit = (qGreen(lines[top][left]) & (1 << bit)) > 0 ? true : false;
            break;
        case 2:
            resultBit = (qBlue(lines[top][left]) & (1 << bit)) > 0 ? true : false;
            break;
        }
        if (resultBit)
            result = result | (1 << i);
        if (!next())
            return result;
    }
    return result;
}

bool ImageLayerCursor::writeByte(uchar b)
{
    for (int i=0; i<8; i++)
    {
        bool resultBit = b & (1 << i);
        int prevColor;
        switch (color)
        {
        default:
        case 0:
            prevColor = qRed(lines[top][left]);
            if (resultBit)
                prevColor ^= int(1 << bit);
            else
                prevColor &= ~(int(1 << bit));
            lines[top][left] = qRgb(prevColor, qGreen(lines[top][left]), qBlue(lines[top][left]));
            break;
        case 1:
            prevColor = qGreen(lines[top][left]);
            if (resultBit)
                prevColor ^= int(1 << bit);
            else
                prevColor &= ~(int(1 << bit));
            lines[top][left] = qRgb(qRed(lines[top][left]), prevColor, qBlue(lines[top][left]));
            break;
        case 2:
            prevColor = qBlue(lines[top][left]);
            if (resultBit)
                prevColor ^= int(1 << bit);
            else
                prevColor &= ~(int(1 << bit));
            lines[top][left] = qRgb(qRed(lines[top][left]), qGreen(lines[top][left]), prevColor);
            break;
        }
        if (!next())
            return false;
    }
}

bool ImageLayerCursor::seek(int value)
{
    while (value)
    {
        if (!next())
            return false;
        else
            value--;
    }
    return true;
}

void ImageLayerCursor::reset()
{
    left = 0;
    top = 0;
    color = 0;
    bit = 0;
    pos = 0;
    width = image->width();
    height = image->height();
}

LSBHider::LSBHider(QObject *parent) : QObject(parent)
{

}


LSBHiderFileSystem LSBHiderFileSystem::fromByteArray(QByteArray data)
{

}

QByteArray LSBHiderFileSystem::toByteArray()
{
    QByteArray result;
    buildHeader();
    result.append(QByteArray::number(magic));
    QList<QString> keys = filePointers.keys();
    foreach (const QString &key, keys)
    {
        DataDesc dd = filePointers[key];
        result.append(QByteArray::number(key.length())); //32
        result.append(key.toLocal8Bit());
        result.append(dd.pointer);
        result.append(dd.size);
    }
    return result;
}

void LSBHiderFileSystem::addFile(QString name, uint size)
{
    DataDesc dd;
    dd.pointer = 0;
    dd.size = size;
    filePointers[name] = dd;
}

void LSBHiderFileSystem::removeFile(QString name)
{
    filePointers.remove(name);
}

void LSBHiderFileSystem::buildHeader()
{
    int initialPointer = 32;
    QList<QString> keys = filePointers.keys();
    foreach (const QString &key, keys)
    {
        filePointers[key].pointer = initialPointer;
        initialPointer += filePointers[key].size * 8;
        initialPointer += 96;
        initialPointer += key.toLocal8Bit().length();
    }
}
