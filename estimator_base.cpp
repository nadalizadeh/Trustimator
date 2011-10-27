#include "estimator_base.h"

EstimatorBase::EstimatorBase()
{
    this->dataset = NULL;
    this->validationDataset = NULL;
    this->is_running = false;
    this->break_when_possible = false;
}

QString EstimatorBase::getName()
{
    return "Base Estimator";
}

/*void EstimatorBase::setupParams()
{
    // Do Nothing
    qFatal("This should not get called");
}
*/
