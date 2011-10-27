#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "estimator_neuralnet1.h"
#include "estimator_neuralnetcascade.h"

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
    ui->dataTable->resizeColumnsToContents();

    this->testDataSet = new Dataset();

    res = this->testDataSet->loadData(prefix + "melli_num.json", prefix + "melli_num_test.csv");
    if (res != DS_ERR_SUCCEED) cerr << "Failed to open file." << endl;

    this->nnet = new EstimatorNeuralNet1();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::check_if_running()
{
    if (nnet->is_running)
    {
        nnet->break_when_possible = true;
        return true;
    }

    return false;
}


void MainWindow::on_goButton_clicked()
{
    if (check_if_running())
        return;

    nnet->dataset = this->dataSet;
    nnet->validationDataset = this->testDataSet;

    unsigned int num_data, num_input, num_output;
    dataSet->get_training_dimensions(&num_data, &num_input, &num_output);
    nnet->createNetwork(num_input, num_output);
    nnet->train();
}

void MainWindow::on_testButton_clicked()
{
    if (check_if_running())
        return;

    ui->dataTable->setModel(testDataSet);

    nnet->dataset = this->testDataSet;

    nnet->loadNetwork();
    nnet->test();
}
