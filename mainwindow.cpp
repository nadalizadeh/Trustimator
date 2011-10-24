#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->dataSet = new Dataset();

    QString prefix = "../../../../";
    int res = this->dataSet->loadData(prefix + "melli_num.json", prefix + "melli_num.csv");
    if (res != DS_ERR_SUCCEED) cerr << "Failed to open file." << endl;

    ui->dataTable->setModel(dataSet);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_goButton_clicked()
{

}
