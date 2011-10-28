#ifndef ESTIMATOR_NEURALNET_H
#define ESTIMATOR_NEURALNET_H

#include "estimator_base.h"

class EstimatorNeuralNet : public EstimatorBase
{
public:
    EstimatorNeuralNet();

    virtual QString getName();

    virtual void train() = 0;
    virtual void createNetwork(int, int) = 0;
    virtual void loadNetwork();
    virtual void test();
    virtual float estimate(float input[]);

protected:
    void turnoff_unneccesary_scales(struct fann * ann, unsigned int num_input, Dataset* dataset);
    struct fann_train_data* getFannData(Dataset* dataset);
    struct fann *ann;
};

#endif // ESTIMATOR_NEURALNET_H
