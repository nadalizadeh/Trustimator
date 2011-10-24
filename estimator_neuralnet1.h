#ifndef ESTIMATOR_NEURALNET1_H
#define ESTIMATOR_NEURALNET1_H

#include "estimator_base.h"

class EstimatorNeuralNet1 : public EstimatorBase
{
public:
    EstimatorNeuralNet1();

    virtual QString getName();
    void train();

private:
    struct fann_train_data* getFannData();
};

#endif // ESTIMATOR_NEURALNET1_H
