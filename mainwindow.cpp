#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
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



void MainWindow::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save container","","PNG (*.png)");
    if (!fileName.isEmpty())
        hider.save(fileName);
}

void MainWindow::on_pushButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter("*.*");
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    foreach (const QString &str, fileNames)
    {
        if (!hider.addFile(str))
            QMessageBox::warning(this, "ERROR", "File " + str + " cant be stored: not enough space");
    }
    drawItems();

    QByteArray firstData;
    firstData.resize(64000);
    hider.cursor->reset();
    for (int i=0; i<64000; i++)
        firstData[i] = hider.cursor->readByte();
    QFile f("data.bin");
    f.open(QFile::WriteOnly);
    f.write(firstData);
    f.flush();
    f.close();
}

void MainWindow::drawItems()
{
    int dataStoredSize = 0;
    ui->listWidget->clear();
    foreach (const LSBHiderFileSystem::DataDesc &dd, hider.header.filePointers)
    {
        ui->listWidget->addItem(dd.name + "\t:\t[" + QString::number(dd.pointer) +"]\t|" + QString::number(dd.size) +" bytes");
        dataStoredSize += dd.size;
    }
    if (dataStoredSize > 0)
    {
        QString totalBytes = QString::number(hider.cursor->state.width * hider.cursor->state.height * 3);
        ui->groupBox->setTitle("LSB found: wrote " +
                               QString::number(dataStoredSize) +
                               "bytes of " + totalBytes + QString(" bytes. [") +
                               QString::number(dataStoredSize*100/totalBytes.toInt()) + "%]");
    }
    else
        ui->groupBox->setTitle("Empty container");
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->listWidget->selectedItems().count() > 0)
        for (int i = 0; i < ui->listWidget->selectedItems().count(); i++)
            hider.removeFile(ui->listWidget->selectedItems()[i]->text());

    drawItems();
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    int num = index.row();
    QString filename = ui->listWidget->item(num)->text();
    QStringList itemTokens = filename.split("\t");
    filename = itemTokens.first();
    QByteArray data = hider.readData(filename);

    if (filename.toLower().contains(".png"))
    {
        QImage img = QImage::fromData(data,"PNG");
        QPixmap pixmap = QPixmap::fromImage(img);
        ui->label->setPixmap(pixmap);
    }
    else if (filename.toLower().contains(".jpg"))
    {
        QImage img = QImage::fromData(data,"JPG");
        QPixmap pixmap = QPixmap::fromImage(img);
        ui->label->setPixmap(pixmap);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PNG container"), "", tr("Image Files (*.png)"));
    if (!fileName.isEmpty())
        hider.loadBackGround(fileName);
    drawItems();

}
