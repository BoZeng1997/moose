//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADComputeThermalExpansionEigenstrainBase.h"
#include "RankTwoTensor.h"

InputParameters
ADComputeThermalExpansionEigenstrainBase::validParams()
{
  InputParameters params = ADComputeEigenstrainBase::validParams();
  params.addCoupledVar("temperature", "Coupled temperature");
  params.addRequiredCoupledVar("stress_free_temperature",
                               "Reference temperature at which there is no "
                               "thermal expansion for thermal eigenstrain "
                               "calculation");
  return params;
}

ADComputeThermalExpansionEigenstrainBase::ADComputeThermalExpansionEigenstrainBase(
    const InputParameters & parameters)
  : ADComputeEigenstrainBase(parameters),
    _temperature(adCoupledValue("temperature")),
    _stress_free_temperature(adCoupledValue("stress_free_temperature"))
{
}

void
ADComputeThermalExpansionEigenstrainBase::computeQpEigenstrain()
{
  ADReal thermal_strain = 0.0;

  computeThermalStrain(thermal_strain);

  _eigenstrain[_qp].zero();
  _eigenstrain[_qp].addIa(thermal_strain);
}
