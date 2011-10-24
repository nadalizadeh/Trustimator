#include "estimator_neuralnet1.h"
#include "fann.h"
#include "dataset.h"
#include <iostream>
#include <cstdio>

using namespace std;

EstimatorNeuralNet1::EstimatorNeuralNet1()
{
}

QString EstimatorNeuralNet1::getName()
{
    return "Neural Network 1";
}

struct fann_train_data* EstimatorNeuralNet1::getFannData()
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

int nn_callback(struct fann *ann, struct fann_train_data */*train*/,
                      unsigned int max_epochs, unsigned int /*epochs_between_reports*/,
                      float /*desired_error*/, unsigned int epochs)
{
    float error = fann_get_MSE(ann);
    fprintf(stderr, "Epochs     %8d/%d. Current error: %.10f. Bit fail %d.\n", epochs, max_epochs, error, ann->num_bit_fail);

    return 0; // -1 to break
}

void EstimatorNeuralNet1::train()
{
    //const unsigned int num_input = 24;
    //const unsigned int num_output = 1;
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 24;
    const unsigned int max_epochs = 4000;
    const unsigned int epochs_between_reports = 50;
    const float desired_error = (const float) 0.001;

    struct fann_train_data * data = NULL;
    data = this->getFannData();

    cerr << "Problem dimensions is " << data->num_input << " x " << data->num_output << endl;
    for(unsigned i=0; i<data->num_input; i++)
    {
        cerr << data->input[0][i] << ",";
    }
    cerr << "  " << data->output[0][0];
    cerr << endl;

    struct fann *ann = fann_create_standard(num_layers, data->num_input, num_neurons_hidden, num_neurons_hidden, data->num_output);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_LINEAR);
    fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
    fann_set_callback(ann, nn_callback);

    fann_set_scaling_params(
            ann,
            data,
            -1,     /* New input minimum */
            1,      /* New input maximum */
            -1,     /* New output minimum */
            1);     /* New output maximum */

    fann_scale_train( ann, data );

    for(unsigned i=0; i<data->num_input; i++)
    {
        cerr << data->input[0][i] << ",";
    }
    cerr << "  " << data->output[0][0];
    cerr << endl;

    fann_train_on_data(ann, data, max_epochs, epochs_between_reports, desired_error);
    free( data );
    fann_save(ann, "../../../german__.net");
    fann_destroy(ann);
}
