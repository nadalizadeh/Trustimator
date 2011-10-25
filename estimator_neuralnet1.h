#ifndef ESTIMATOR_NEURALNET1_H
#define ESTIMATOR_NEURALNET1_H

#include "estimator_neuralnet.h"

class EstimatorNeuralNet1 : public EstimatorNeuralNet
{
public:
    EstimatorNeuralNet1();

    virtual QString getName();
    void setupParams();

    virtual void loadNetwork();
    virtual void createNetwork(int num_input, int num_output);
    virtual void train();
    virtual void test();
};

#endif // ESTIMATOR_NEURALNET1_H
