#ifndef ESTIMATOR_NEURALNET1_H
#define ESTIMATOR_NEURALNET1_H

#include "estimator_neuralnet.h"

class EstimatorNeuralNet1 : public EstimatorNeuralNet
{
public:
    EstimatorNeuralNet1();

    virtual QString getName();
    void setupParams();

    virtual void createNetwork(int num_input, int num_output);
    virtual void train();
    int net_callback(struct fann *ann, struct fann_train_data */*train*/,
                     unsigned int max_epochs, unsigned int /*epochs_between_reports*/,
                     float /*desired_error*/, unsigned int epochs);
protected:
    struct fann_train_data * val_data;
};

#endif // ESTIMATOR_NEURALNET1_H
