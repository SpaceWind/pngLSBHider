#include <QDataStream>
#include <QFile>
#include "lsbhider.h"

ImageLayerCursor::ImageLayerCursor(QImage *img)
{
    image = img;
    state.left = 0;
    state.top = 0;
    state.color = 0;
    state.bit = 0;
    state.pos = 0;
    state.width = img->width();
    state.height = img->height();
    lines = new QRgb*[img->height()];
    for (int i=0; i<img->height(); i++)
        lines[i] = (QRgb*)img->scanLine(i);
}

ImageLayerCursor::~ImageLayerCursor()
{
    delete[] lines;
}

bool ImageLayerCursor::next()
{
    int prev = state.color;
    state.pos++;
    state.color = (state.color +1) % 3;
    if (state.color < prev)
    {
        prev = state.left;
        state.left = (state.left + 1) % state.width;
        if (state.left < prev)
        {
            prev = state.top;
            state.top = (state.top + 1) % state.height;
            if (state.top < prev)
            {
                prev = state.bit;
                state.bit = (state.bit + 1) % 8;
                if (state.bit < prev)
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
        switch (state.color)
        {
        default:
        case 0:
            resultBit = (qRed(lines[state.top][state.left]) & (1 << state.bit)) > 0 ? true : false;
            break;
        case 1:
            resultBit = (qGreen(lines[state.top][state.left]) & (1 << state.bit)) > 0 ? true : false;
            break;
        case 2:
            resultBit = (qBlue(lines[state.top][state.left]) & (1 << state.bit)) > 0 ? true : false;
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
        switch (state.color)
        {
        default:
        case 0:
            prevColor = qRed(lines[state.top][state.left]);
            if (resultBit)
                prevColor ^= int(1 << state.bit);
            else
                prevColor &= ~(int(1 << state.bit));
            lines[state.top][state.left] = qRgb(prevColor, qGreen(lines[state.top][state.left]), qBlue(lines[state.top][state.left]));
            break;
        case 1:
            prevColor = qGreen(lines[state.top][state.left]);
            if (resultBit)
                prevColor ^= int(1 << state.bit);
            else
                prevColor &= ~(int(1 << state.bit));
            lines[state.top][state.left] = qRgb(qRed(lines[state.top][state.left]), prevColor, qBlue(lines[state.top][state.left]));
            break;
        case 2:
            prevColor = qBlue(lines[state.top][state.left]);
            if (resultBit)
                prevColor ^= int(1 << state.bit);
            else
                prevColor &= ~(int(1 << state.bit));
            lines[state.top][state.left] = qRgb(qRed(lines[state.top][state.left]), qGreen(lines[state.top][state.left]), prevColor);
            break;
        }
        if (!next())
            return false;
    }
    return true;
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
    state.left = 0;
    state.top = 0;
    state.color = 0;
    state.bit = 0;
    state.pos = 0;
    state.width = image->width();
    state.height = image->height();
}

void ImageLayerCursor::cacheState(QString name)
{
    cache[name] = state;
}

void ImageLayerCursor::restoreState(QString name)
{
    if (cache.contains(name))
        state = cache[name];
}

LSBHider::LSBHider(QObject *parent) : QObject(parent)
{
    cursor = 0;
    image = 0;
}

void LSBHider::loadBackGround(QString filename)
{
    image = new QImage(filename, "PNG");
    cursor = new ImageLayerCursor(image);
    QByteArray data;
    for (int i = 0; i < 32768; i++)
    {
        data.append(cursor->readByte());
    }
    header = LSBHiderFileSystem::fromByteArray(data);
}

void LSBHider::cacheFiles()
{
    cursor->reset();
    for (auto i = header.filePointers.begin(); i != header.filePointers.end(); ++i)
    {
        cursor->seek((*i).pointer);
        cursor->cacheState((*i).name);
        cursor->reset();
    }
}

bool LSBHider::addFile(QString filename)
{
    QByteArray data;
    QFile file(filename);
    file.open(QFile::ReadOnly);
    data.append(file.readAll());
    file.close();
    return writeData(data, filename);
}

bool LSBHider::writeData(QByteArray data, QString name)
{
    header.addFile(name,data.size());
    header.buildHeader();
    int pointer = header.getFilePointer(name);
    cursor->reset();
    cursor->seek(pointer);
    cursor->cacheState(name);
    for (int i = 0; i< data.count(); i++)
    {
        if (!cursor->writeByte(data[i]))
        {
            header.removeFile(name);
            return false;
        }
    }
    return true;
}

void LSBHider::save(QString filename)
{
    cursor->reset();
    QByteArray headerData = header.toByteArray();
    for (int i=0; i < headerData.size(); i++)
        cursor->writeByte(headerData[i]);
    image->save(filename, "PNG");
}

QByteArray LSBHider::readData(QString name)
{
    QByteArray result;
    foreach (const LSBHiderFileSystem::DataDesc &dd, header.filePointers)
        if (dd.name == name)
        {
            int size = dd.size;
            int pointer = dd.pointer;
            cursor->reset();
            cursor->seek(pointer);
            for (int i = 0; i<size; i++)
                result.append(cursor->readByte());
        }
    return result;
}

LSBHider::~LSBHider()
{
    if (image)
        delete image;
    if (cursor)
        delete cursor;
}


LSBHiderFileSystem LSBHiderFileSystem::fromByteArray(QByteArray data)
{
    LSBHiderFileSystem result;
    QDataStream ds(&data, QIODevice::ReadOnly);
    int givenMagic = 0;
    ds >> givenMagic;
    if (result.magic != givenMagic)
        return result;
    int cnt;
    ds >> cnt;
    for (int i=0; i<cnt; i++)
    {
        DataDesc dd;
        ds >> dd.name;
        ds >> dd.pointer;
        ds >> dd.size;
        result.filePointers.append(dd);
    }
    return result;
}

QByteArray LSBHiderFileSystem::toByteArray()
{
    QByteArray result;
    QDataStream ds(&result, QIODevice::WriteOnly);
    buildHeader();
    ds << magic;
    ds << filePointers.count();
    foreach (const DataDesc & dd, filePointers)
    {
        ds << dd.name << dd.pointer << dd.size;
    }
    return result;
}

void LSBHiderFileSystem::addFile(QString name, uint size)
{
    DataDesc dd;
    dd.pointer = 0;
    dd.size = size;
    dd.name = name;
    for (int i = 0; i< filePointers.count(); ++i)
        if (filePointers[i].name == name)
        {
            filePointers.removeAt(i);
            break;
        }
    filePointers.append(dd);
}

void LSBHiderFileSystem::removeFile(QString name)
{
    for (int i = 0; i< filePointers.count(); ++i)
        if (filePointers[i].name == name)
        {
            filePointers.removeAt(i);
            break;
        }
}

void LSBHiderFileSystem::buildHeader()
{
    int initialPointer = 64;
    foreach (const DataDesc & dd, filePointers)
    {
        initialPointer += 32;
        initialPointer += dd.name.size() * 2 * 8;
        initialPointer += sizeof(dd.pointer) * 8;
        initialPointer += sizeof(dd.size) * 8;
    }

    for (auto i = filePointers.begin(); i!= filePointers.end(); ++i)
    {
        (*i).pointer = initialPointer;
        initialPointer += (*i).size * 8;
    }
}

int LSBHiderFileSystem::getFilePointer(QString name)
{
    buildHeader();
    for (int i = 0; i< filePointers.count(); ++i)
        if (filePointers[i].name == name)
            return filePointers[i].pointer;
    return -1;
}
