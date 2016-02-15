#ifndef LSBHIDER_H
#define LSBHIDER_H

#include <QObject>
#include <QImage>
#include <QHash>
#include <QByteArray>

struct LSBHiderFileSystem
{
    struct DataDesc
    {
        int pointer;
        int size;
    };
    static LSBHiderFileSystem fromByteArray(QByteArray data);

    QByteArray toByteArray();

    const uint magic = 0xF0F0E0C0;
    QHash<QString, DataDesc> filePointers;
    void addFile(QString name, uint size);
    void removeFile(QString name);
    void buildHeader();
};


struct ImageLayerCursor
{
    ImageLayerCursor(QImage* img);
    ~ImageLayerCursor();

    bool next();
    uchar readByte();
    bool writeByte(uchar b);

    bool seek(int value);
    void reset();

    int left;
    int top;
    int color;
    int bit;
    int pos;
private:
    int width;
    int height;
    QImage *image;
    QRgb** lines;
};

class LSBHider : public QObject
{
    Q_OBJECT
public:
    explicit LSBHider(QObject *parent = 0);

signals:

public slots:
};

#endif // LSBHIDER_H
