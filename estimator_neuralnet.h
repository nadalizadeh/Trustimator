#ifndef ESTIMATOR_NEURALNET_H
#define ESTIMATOR_NEURALNET_H

#include "estimator_base.h"

class EstimatorNeuralNet : public EstimatorBase
{
public:
    EstimatorNeuralNet();

    virtual QString getName();

    virtual void train() = 0;
    virtual void loadNetwork() = 0;
    virtual void createNetwork(int, int) = 0;
    virtual void test() = 0;

protected:
    struct fann_train_data* getFannData();
    struct fann *ann;
};

#endif // ESTIMATOR_NEURALNET_H
