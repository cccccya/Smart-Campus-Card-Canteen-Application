#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QInputDialog>
#include "log.h"
#include "manager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void GetDataWidget();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    void Reflush1();
    void Reflush2();
    void ReflushTime();
};

#endif // MAINWINDOW_H
