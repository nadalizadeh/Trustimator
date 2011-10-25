#include "estimator_neuralnet.h"
#include "fann.h"
#include "dataset.h"
#include <iostream>
#include <cstdio>

EstimatorNeuralNet::EstimatorNeuralNet() : EstimatorBase()
{
    this->ann = NULL;
}

QString EstimatorNeuralNet::getName()
{
    return "Neural Network";
}

struct fann_train_data* EstimatorNeuralNet::getFannData()
{
    unsigned int num_input, num_output, num_data, i;
    fann_type *data_input, *data_output;
    struct fann_train_data *data =
            (struct fann_train_data *) malloc(sizeof(struct fann_train_data));

    if(data == NULL)
    {
        fann_error(NULL, FANN_E_CANT_ALLOCATE_MEM);
        return NULL;
    }

    this->dataset->get_training_dimensions(&num_data, &num_input, &num_output);

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

        this->dataset->get_training_row(i, data->input[i], data->output[i]);
    }

    return data;
}

