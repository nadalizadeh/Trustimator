#include "estimator_neuralnetcascade.h"
#include "fann.h"
#include "dataset.h"
#include <iostream>
#include <cstdio>
#include <cassert>
#include <QApplication>

using namespace std;

EstimatorNeuralNetCascade::EstimatorNeuralNetCascade() : EstimatorNeuralNet()
{
    setupParams();
    val_data = NULL;
}

void EstimatorNeuralNetCascade::setupParams()
{
    cerr << "setup" << endl;
    params["network_def"] = QVariant("../../../../x.credit_scoring.cascade.net");
}


QString EstimatorNeuralNetCascade::getName()
{
    return "Cascade Neural Network";
}

int cc_nn_callback(struct fann *ann, struct fann_train_data * train,
                      unsigned int max_epochs, unsigned int epochs_between_reports,
                      float desired_error, unsigned int epochs)
{
    void * user_data = fann_get_user_data(ann);
    EstimatorNeuralNetCascade* nnetobj = (EstimatorNeuralNetCascade*) user_data;

    if (nnetobj != NULL)
        return nnetobj->net_callback(ann, train, max_epochs, epochs_between_reports, desired_error, epochs);

    return 0;
}

int EstimatorNeuralNetCascade::net_callback(struct fann *ann, struct fann_train_data */*train*/,
                 unsigned int max_neurons, unsigned int /*neurons_between_reports*/,
                 float /*desired_error*/, unsigned int total_epochs)
{
    float error = fann_get_MSE(ann);
    fprintf(stderr, "Neurons     . Current error: %.6f. Total error:%8.4f. Epochs %5d. Bit fail %3d",
            error, ann->MSE_value, total_epochs, ann->num_bit_fail);

    if((ann->last_layer-2) != ann->first_layer)
    {
        fprintf(stderr, ". candidate steepness %.2f. function %s", (ann->last_layer-2)->first_neuron->activation_steepness,
                FANN_ACTIVATIONFUNC_NAMES[(ann->last_layer-2)->first_neuron->activation_function]);
    }

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

    QApplication::processEvents();

    return 0; // -1 to break
}

void EstimatorNeuralNetCascade::loadNetwork()
{
    char * network_def = params["network_def"].toString().toUtf8().data();
    this->ann = fann_create_from_file(network_def);
}

void EstimatorNeuralNetCascade::createNetwork(int num_input, int num_output)
{
    unsigned int layers[2] = {num_input, num_output};
    this->ann = fann_create_shortcut_array(2, layers);

    // SIGMOID, SIGMOID_SYMMETRIC
    fann_set_learning_rate(ann, 0.1);
    fann_set_quickprop_decay(ann, 0.0);
    fann_set_quickprop_mu(ann, 2.0);
    fann_set_cascade_weight_multiplier(ann, 1);
    fann_set_cascade_max_out_epochs(ann, 150);
    fann_set_bit_fail_limit(ann, 0.35);
    fann_set_activation_steepness_output(ann, 1);

    fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);

    fann_set_activation_function_output(ann, FANN_LINEAR_PIECE);
    fann_set_activation_function_output(ann, FANN_LINEAR_PIECE_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_LINEAR);

    fann_set_train_error_function(ann, FANN_ERRORFUNC_TANH);
    fann_set_train_error_function(ann, FANN_ERRORFUNC_LINEAR);


    fann_randomize_weights(ann, 0.1, 0.1);

/*
    fann_set_cascade_weight_multiplier(ann, 0.4);
    fann_set_cascade_candidate_limit(ann, 1000.0);
    */
    fann_set_cascade_output_change_fraction(ann, 0.01);
    fann_set_cascade_candidate_change_fraction(ann, 0.01);

    /*
    steepnesses = (fann_type *)calloc(1,  sizeof(fann_type));
    steepnesses[0] = (fann_type)1;
    fann_set_cascade_activation_steepnesses(ann, steepnesses, 1);
    */
    fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
}

void EstimatorNeuralNetCascade::test()
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

void EstimatorNeuralNetCascade::train()
{
    const unsigned int max_neurons = 30;
    const unsigned int neurons_between_reports = 1;
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
            1,     /* New input maximum */
            0,     /* New output minimum */
            1);    /* New output maximum */

    fann_set_callback(ann, cc_nn_callback);
    fann_set_user_data(ann, this);
    fann_scale_train( ann, data );

    for(unsigned i=0; i<data->num_input; i++)
    {
        cerr << data->input[0][i] << ",";
    }
    cerr << "  " << data->output[0][0];
    cerr << endl;

    fann_cascadetrain_on_data(ann, data, max_neurons, neurons_between_reports, desired_error);

    fann_save(ann, params["network_def"].toString().toUtf8().data());

    fann_destroy_train(data);
    fann_destroy(ann);
}
