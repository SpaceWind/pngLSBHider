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
        QString name;
        int pointer;
        int size;
    };
    static LSBHiderFileSystem fromByteArray(QByteArray data);

    QByteArray toByteArray();

    static const int magic = 0xF0F0E0C0;
    QList<DataDesc> filePointers;
    void addFile(QString name, uint size);
    void removeFile(QString name);
    void buildHeader();
    int getFilePointer(QString name);
};


struct ImageLayerCursor
{
    ImageLayerCursor(QImage* img);
    ~ImageLayerCursor();

    struct ImageLayerCursorState
    {
        int left, top, color, bit, pos, width, height;
    };

    bool next();
    uchar readByte();
    bool writeByte(uchar b);

    bool seek(int value);
    void reset();
    void cacheState(QString name);
    void restoreState(QString name);

    ImageLayerCursorState state;
private:
    QImage *image;
    QRgb** lines;
    QHash<QString, ImageLayerCursorState> cache;
};

class LSBHider : public QObject
{
    Q_OBJECT
public:
    explicit LSBHider(QObject *parent = 0);
    void loadBackGround(QString filename);
    void cacheFiles();
    bool addFile(QString filename);
    bool writeData(QByteArray data, QString name);
    void save(QString filename);
    QByteArray readData(QString name);
    ~LSBHider();

signals:

public slots:
private:
    QImage * image;
    ImageLayerCursor * cursor;
    LSBHiderFileSystem header;
};

#endif // LSBHIDER_H
