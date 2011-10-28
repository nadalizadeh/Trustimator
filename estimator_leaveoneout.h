#ifndef ESTIMATOR_LEAVEONEOUT_H
#define ESTIMATOR_LEAVEONEOUT_H

#include <estimator_neuralnet1.h>

class EstimatorLeaveOneOut : public EstimatorNeuralNet1
{
public:
    EstimatorLeaveOneOut();

    virtual void train();
    int net_callback(struct fann *ann, struct fann_train_data */*train*/,
                     unsigned int max_epochs, unsigned int /*epochs_between_reports*/,
                     float /*desired_error*/, unsigned int epochs);
protected:
    float * mse_list;
    float * val_input;
    float * val_output;
    unsigned int leaving_row;
};

#endif // ESTIMATOR_LEAVEONEOUT_H
