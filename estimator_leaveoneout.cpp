#include "estimator_leaveoneout.h"
#include "fann.h"
#include "dataset.h"
#include <iostream>
#include <cstdio>
#include <cassert>
#include <QApplication>

using namespace std;

EstimatorLeaveOneOut::EstimatorLeaveOneOut() : EstimatorNeuralNet1()
{
    leaving_row = DONT_LEAVE_ANY;
}

int loo_callback(struct fann *ann, struct fann_train_data * train,
                      unsigned int max_epochs, unsigned int epochs_between_reports,
                      float desired_error, unsigned int epochs)
{
    void * user_data = fann_get_user_data(ann);
    EstimatorLeaveOneOut* nnetobj = (EstimatorLeaveOneOut*) user_data;

    if (nnetobj != NULL)
        return nnetobj->net_callback(ann, train, max_epochs, epochs_between_reports, desired_error, epochs);

    return 0;
}

int EstimatorLeaveOneOut::net_callback(struct fann *ann, struct fann_train_data */*train*/,
                 unsigned int /*max_epochs*/, unsigned int /*epochs_between_reports*/,
                 float /*desired_error*/, unsigned int epochs)
{
    if (this->break_when_possible)
    {
        this->break_when_possible = false;
        cerr << "Breaking the training ..." << endl;
        return -1;
    }

//    float error = fann_get_MSE(ann);
//    fprintf(stderr, "Epochs   %8d/%d. Current error: %.10f. Bit fail %d. ", epochs, max_epochs, error, ann->num_bit_fail);

    val_data = this->getFannData(this->validationDataset);

    fann_reset_MSE(ann);
    fann_type *calc_out;

    calc_out = fann_run( ann, val_input );
    fann_compute_MSE(ann, val_output);
    //        fann_descale_output( ann, calc_out );
    //        fann_descale_output( ann, val_output );

    //        average_error += (float) fann_abs(calc_out[0] - val_data->output[i][0]);

    //        fprintf(stderr, "  validation avg-err : %f   MSE: %f\n", average_error, fann_get_MSE(ann));
    if (mse_list[leaving_row] > fann_get_MSE(ann))
    {
        mse_list[leaving_row] = fann_get_MSE(ann);
        cerr << epochs << ": Better MSE for " << leaving_row << " : " << mse_list[leaving_row] << endl;
    }

    fann_reset_MSE(ann);

    QApplication::processEvents();

    return 0; // -1 to break
}

void EstimatorLeaveOneOut::train()
{
    const unsigned int max_epochs = 100;
    const unsigned int epochs_between_reports = 5;
    const float desired_error = (const float) 0.001;

    this->is_running = true;
    this->break_when_possible = false;

    struct fann_train_data * data = NULL;

    unsigned int total_num_data, num_input, num_output;
    dataset->get_training_dimensions(&total_num_data, &num_input, &num_output);

    val_input = new float[num_input];
    val_output = new float[num_output];
    mse_list = new float[total_num_data];

    for( leaving_row = 0; leaving_row < total_num_data; leaving_row++ )
    {
        dataset->leave_item_out(DONT_LEAVE_ANY);

        // Get the row
        dataset->get_training_row(leaving_row, val_input, val_output);

        // Now hide it :)
        dataset->leave_item_out(leaving_row);

        mse_list[leaving_row] = 99999999999999.0; // Lazy! use FLOAT_MAX when found it :)
        data = this->getFannData(this->dataset);

        assert(data != NULL);

        cerr << "Problem dimensions is " << data->num_input << " x " << data->num_output << endl;
        for(unsigned i=0; i<data->num_input; i++)
        {
            cerr << data->input[0][i] << ",";
        }
        cerr << "  " << data->output[0][0];
        cerr << endl;

        this->createNetwork(num_input, num_output);
        fann_set_scaling_params(
                ann,
                data,
                0,     /* New input minimum */
                1,     /* New input maximum */
                0,     /* New output minimum */
                1);    /* New output maximum */

        turnoff_unneccesary_scales(ann, data->num_input, dataset);

        fann_set_callback(ann, loo_callback);
        fann_set_user_data(ann, this);
        fann_scale_train( ann, data );

        // Scale Validation Row (The row we've left)
        fann_scale_input( ann, val_input );
        fann_scale_output( ann, val_output );

        for(unsigned i=0; i<data->num_input; i++)
        {
            cerr << data->input[0][i] << ",";
        }
        cerr << "  " << data->output[0][0];
        cerr << endl;

        fann_train_on_data(ann, data, max_epochs, epochs_between_reports, desired_error);
        free( data );

//        cerr << "Saving the network ..." << endl;
//        fann_save(ann, params["network_def"].toString().toUtf8().data());

        this->is_running = false;

        fann_destroy(ann);
        ann = NULL;
    }

    // Average Error
    cerr << "--MSE LIST--" << endl;
    float sum = 0.0f;
    for(unsigned i=0; i<total_num_data; i++)
    {
        sum += mse_list[i];
        cerr << mse_list[i] << endl;
    }
    cerr << "--AVERAGE MSE--" << endl;
    cerr << sum / total_num_data << endl;

    delete[] val_input;
    delete[] val_output;
    delete[] mse_list;
}
