#ifndef ESTIMATOR_BASE_H
#define ESTIMATOR_BASE_H

#include <QString>
#include <QMap>
#include <QVariant>

class Dataset;

class EstimatorBase
{
public:
    EstimatorBase();

    virtual QString getName();
    virtual void setupParams() = 0;
    Dataset* dataset;
    Dataset* validationDataset;

public:
    QMap<QString, QVariant> params;
    bool is_running;
    bool break_when_possible;
};

#endif // ESTIMATOR_BASE_H
