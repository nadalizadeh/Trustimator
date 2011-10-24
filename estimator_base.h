#ifndef ESTIMATOR_BASE_H
#define ESTIMATOR_BASE_H

#include <QString>

class Dataset;

class EstimatorBase
{
public:
    EstimatorBase();

    virtual QString getName();
    Dataset* dataset;
};

#endif // ESTIMATOR_BASE_H
