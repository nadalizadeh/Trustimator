#include "estimator_base.h"

EstimatorBase::EstimatorBase()
{
    this->dataset = NULL;
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
