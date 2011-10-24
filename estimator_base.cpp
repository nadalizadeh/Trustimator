#include "estimator_base.h"

EstimatorBase::EstimatorBase()
{
    this->dataset = NULL;
}

QString EstimatorBase::getName()
{
    return "Base Estimator";
}
