#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataset.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Dataset* dataSet;
    Dataset* testDataSet;

private slots:
    void on_testButton_clicked();
    void on_goButton_clicked();
};

#endif // MAINWINDOW_H
