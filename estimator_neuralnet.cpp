#include "estimator_neuralnet.h"
#include "fann.h"
#include "dataset.h"
#include <iostream>
#include <cstdio>

using namespace std;

EstimatorNeuralNet::EstimatorNeuralNet() : EstimatorBase()
{
    this->ann = NULL;
}

QString EstimatorNeuralNet::getName()
{
    return "Neural Network";
}

void EstimatorNeuralNet::loadNetwork()
{
    char * network_def = params["network_def"].toString().toUtf8().data();
    this->ann = fann_create_from_file(network_def);
}

struct fann_train_data* EstimatorNeuralNet::getFannData(Dataset* dset)
{
    if (dset == NULL) return NULL;

    unsigned int num_input, num_output, num_data, i;
    fann_type *data_input, *data_output;
    struct fann_train_data *data =
            (struct fann_train_data *) malloc(sizeof(struct fann_train_data));

    if(data == NULL)
    {
        fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
        return NULL;
    }

    dset->get_training_dimensions(&num_data, &num_input, &num_output);

    // cerr << "Initializing using " << num_data << " i " << num_input << endl;
    fann_init_error_data((struct fann_error *) data);

    data->num_data = num_data;
    data->num_input = num_input;
    data->num_output = num_output;
    data->input = (fann_type **) calloc(num_data, sizeof(fann_type *));
    if(data->input == NULL)
    {
        fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
        fann_destroy_train(data);
        return NULL;
    }

    data->output = (fann_type **) calloc(num_data, sizeof(fann_type *));
    if(data->output == NULL)
    {
        fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
        fann_destroy_train(data);
        return NULL;
    }

    data_input = (fann_type *) calloc(num_input * num_data, sizeof(fann_type));
    if(data_input == NULL)
    {
        fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
        fann_destroy_train(data);
        return NULL;
    }

    data_output = (fann_type *) calloc(num_output * num_data, sizeof(fann_type));
    if(data_output == NULL)
    {
        fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
        fann_destroy_train(data);
        return NULL;
    }

    for(i = 0; i != num_data; i++)
    {
        data->input[i] = data_input;
        data_input += num_input;

        data->output[i] = data_output;
        data_output += num_output;

        dset->get_training_row(i, data->input[i], data->output[i]);
    }

    return data;
}

void EstimatorNeuralNet::turnoff_unneccesary_scales(struct fann * ann, unsigned int num_input, Dataset* dataset)
{
    int * should_scale = new int[num_input];

    dataset->get_scale_flags(should_scale);

    for(unsigned i = 0; i < num_input; i++)
    {
        if (should_scale[i] == 0)
        {
            ann->scale_new_min_in[i] = -1.0f; // for the odd (-1.0) in scale function
            ann->scale_factor_in[i] = 1.0f;
            ann->scale_deviation_in[i] = 1.0f;
            ann->scale_mean_in[i] = 0.0;
        }
    }

    delete[] should_scale;
}

void EstimatorNeuralNet::test()
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
    fann_destroy(ann);
}

float EstimatorNeuralNet::estimate(float input[])
{
    if (ann == NULL)
        this->loadNetwork();

    fann_scale_input( ann, input );
    fann_type* calc_out = fann_run( ann, input );
    fann_descale_output( ann, calc_out );

    return calc_out[0];
}
