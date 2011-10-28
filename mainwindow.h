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
    class EstimatorNeuralNet* nnet;

private slots:
    void on_singleEstimateButton_clicked();
    void on_testButton_clicked();
    void on_goButton_clicked();

private:
    bool check_if_running();
};

#endif // MAINWINDOW_H
