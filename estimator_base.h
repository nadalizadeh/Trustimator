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
};

#endif // ESTIMATOR_BASE_H
