#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lsbhider.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:


    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void drawItems();

    void on_pushButton_2_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;

    LSBHider hider;

};

#endif // MAINWINDOW_H
