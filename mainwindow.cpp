#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "diffimage.h"
#include "lsbhider.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    src = QImage("file.png","PNG");
    out = QImage("file.png","PNG");
    premultOut = QImage("file.png","PNG");
    diff = DiffImage::createQuantDeltaEncoded(src,out,ui->spinBox->value());
    out = DiffImage::recoverQuantDeltaEncoded(diff,ui->spinBox->value());
    ui->label->setPixmap(QPixmap::fromImage(out));
}

void MainWindow::on_radioButton_clicked()
{
    ui->label->setPixmap(QPixmap::fromImage(src));
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->label->setPixmap(QPixmap::fromImage(diff));
}

void MainWindow::on_radioButton_3_clicked()
{
    ui->label->setPixmap(QPixmap::fromImage(out));
}

void MainWindow::on_pushButton_2_clicked()
{
    QImage rgbImage = diff.convertToFormat(QImage::Format_RGB888);
    int size = rgbImage.byteCount();
    QByteArray data = qCompress(QByteArray::fromRawData((char*)rgbImage.bits(),size),9);
    out.save("dec.png","PNG");
    diff.save("diff.png","PNG");
    QFile outFile("file.pngd");
    outFile.open(QFile::WriteOnly);
    outFile.write(data);
    outFile.flush();
    outFile.close();

    QFileInfo srcFileInfo("file.png");
    int srcSize = srcFileInfo.size();
    QMessageBox::information(this, "File Saved", "DIFF file size: " + QString::number(data.size()) + " bytes. Original file size: " + QString::number(srcSize) + " bytes.");
}

void MainWindow::on_radioButton_4_clicked()
{
    ui->label->setPixmap(QPixmap::fromImage(premultDiff));
}

void MainWindow::on_radioButton_5_clicked()
{
    ui->label->setPixmap(QPixmap::fromImage(premultOut));
}

void MainWindow::on_pushButton_4_clicked()
{
    diff = QImage("diff.png","PNG");
    out = DiffImage::recoverQuantDeltaEncoded(diff,4);
}

void MainWindow::on_pushButton_5_clicked()
{
    LSBHider lsbHider(this);
    lsbHider.loadBackGround("file.png");


    lsbHider.addFile("1.jpg");
    lsbHider.save("super.png");

    lsbHider.loadBackGround("super.png");
    QByteArray outputData;
    for (int i=0; i< 1024; i++)
        outputData.append(char(0));
    lsbHider.cursor->reset();
    for (int i=0; i< 1024; i++)
        outputData[i] = lsbHider.cursor->readByte();
    QFile outputDataFile("output.dat");
    outputDataFile.open(QFile::WriteOnly);
    outputDataFile.write(outputData);
    outputDataFile.flush();
    outputDataFile.close();
}
