#ifndef SINGLE_ESTIMATE_DIALOG_H
#define SINGLE_ESTIMATE_DIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
    class SingleEstimateDialog;
}

class Dataset;
class EstimatorBase;

class SingleEstimateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SingleEstimateDialog(QWidget *parent, Dataset *ds, EstimatorBase* estm);
    ~SingleEstimateDialog();

private slots:
    void createForm();
    void updateEstimate();

private:
    Ui::SingleEstimateDialog *ui;
    Dataset* dataset;
    EstimatorBase* estimator;
    QList<QWidget*> input_widgets;
};

#endif // SINGLE_ESTIMATE_DIALOG_H
