#include "estimator_neuralnet1.h"
#include "fann.h"
#include "dataset.h"
#include <iostream>
#include <cstdio>
#include <cassert>

using namespace std;

EstimatorNeuralNet1::EstimatorNeuralNet1() : EstimatorNeuralNet()
{
    setupParams();
    val_data = NULL;
}

void EstimatorNeuralNet1::setupParams()
{
    cerr << "setup" << endl;
    params["network_def"] = QVariant("../../../../x.credit_scoring.net");
}


QString EstimatorNeuralNet1::getName()
{
    return "Neural Network 1";
}

int nn_callback(struct fann *ann, struct fann_train_data * train,
                      unsigned int max_epochs, unsigned int epochs_between_reports,
                      float desired_error, unsigned int epochs)
{
    void * user_data = fann_get_user_data(ann);
    EstimatorNeuralNet1* nnetobj = (EstimatorNeuralNet1*) user_data;

    if (nnetobj != NULL)
        return nnetobj->net_callback(ann, train, max_epochs, epochs_between_reports, desired_error, epochs);

    return 0;
}

int EstimatorNeuralNet1::net_callback(struct fann *ann, struct fann_train_data */*train*/,
                 unsigned int max_epochs, unsigned int /*epochs_between_reports*/,
                 float /*desired_error*/, unsigned int epochs)
{
    float error = fann_get_MSE(ann);
    fprintf(stderr, "Epochs   %8d/%d. Current error: %.10f. Bit fail %d. ", epochs, max_epochs, error, ann->num_bit_fail);

    val_data = this->getFannData(this->validationDataset);
    if (val_data)
    {
        fann_reset_MSE(ann);
        fann_type *calc_out;
        float average_error = 0;

        for(unsigned i = 0; i < fann_length_train_data(val_data); i++)
        {
            {
                fann_scale_input( ann, val_data->input[i] );
                fann_scale_output( ann, val_data->output[i] );
                calc_out = fann_run( ann, val_data->input[i] );
                fann_compute_MSE(ann, val_data->output[i]);
                fann_descale_output( ann, calc_out );
                fann_descale_output( ann, val_data->output[i] );
            }

            average_error += (float) fann_abs(calc_out[0] - val_data->output[i][0]);
/*            fprintf(stderr, "Result %f original %f error %f\n",
                   calc_out[0], data->output[i][0],
                   (float) fann_abs(calc_out[0] - data->output[i][0]));
 */     }
        average_error /= fann_length_train_data(val_data);

        fprintf(stderr, "  validation avg-err : %f   MSE: %f\n", average_error, fann_get_MSE(ann));
        fann_reset_MSE(ann);
    } else {
        fprintf(stderr, "\n");
    }

    return 0; // -1 to break
}

void EstimatorNeuralNet1::loadNetwork()
{
    char * network_def = params["network_def"].toString().toUtf8().data();
    this->ann = fann_create_from_file(network_def);
}

void EstimatorNeuralNet1::createNetwork(int num_input, int num_output)
{
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 16;

    this->ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_neurons_hidden, num_output);

    // SIGMOID, SIGMOID_SYMMETRIC
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_LINEAR);

    // INCREMENTAL, BATCH, RPROP, QUICKPROP
    fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
}

void EstimatorNeuralNet1::test()
{
    cerr << "ENN::test()" << endl;

    struct fann_train_data * data = NULL;
    data = this->getFannData(this->dataset);

    fann_type *calc_out;
    float average_error = 0;

    for(unsigned i = 0; i < fann_length_train_data(data); i++)
    {
        fann_reset_MSE(ann);
        {
            fann_scale_input( ann, data->input[i] );
            calc_out = fann_run( ann, data->input[i] );
            fann_descale_output( ann, calc_out );
        }

        average_error += (float) fann_abs(calc_out[0] - data->output[i][0]);
        fprintf(stderr, "Result %f original %f error %f\n",
               calc_out[0], data->output[i][0],
               (float) fann_abs(calc_out[0] - data->output[i][0]));
    }
    average_error /= fann_length_train_data(data);

    fprintf(stderr, "Average Error : %f\n", average_error);
    fann_destroy_train(data);
    if (val_data)
    {
        fann_destroy_train(val_data);
        val_data = NULL;
    }
    fann_destroy(ann);
}

void EstimatorNeuralNet1::train()
{
    const unsigned int max_epochs = 3000;
    const unsigned int epochs_between_reports = 10;
    const float desired_error = (const float) 0.001;

    struct fann_train_data * data = NULL;
    data = this->getFannData(this->dataset);

    assert(data != NULL);

    cerr << "Problem dimensions is " << data->num_input << " x " << data->num_output << endl;
    for(unsigned i=0; i<data->num_input; i++)
    {
        cerr << data->input[0][i] << ",";
    }
    cerr << "  " << data->output[0][0];
    cerr << endl;

    fann_set_scaling_params(
            ann,
            data,
            0,     /* New input minimum */
            1,      /* New input maximum */
            0,     /* New output minimum */
            1);     /* New output maximum */

    fann_set_callback(ann, nn_callback);
    fann_set_user_data(ann, this);
    fann_scale_train( ann, data );

    for(unsigned i=0; i<data->num_input; i++)
    {
        cerr << data->input[0][i] << ",";
    }
    cerr << "  " << data->output[0][0];
    cerr << endl;

    fann_train_on_data(ann, data, max_epochs, epochs_between_reports, desired_error);
    free( data );

    fann_save(ann, params["network_def"].toString().toUtf8().data());
    fann_destroy(ann);
}
