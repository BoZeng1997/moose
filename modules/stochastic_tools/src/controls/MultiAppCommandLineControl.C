//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "MultiAppCommandLineControl.h"
#include "Function.h"
#include "Sampler.h"
#include "MultiApp.h"
#include "SamplerFullSolveMultiApp.h"

registerMooseObject("StochasticToolsApp", MultiAppCommandLineControl);

InputParameters
MultiAppCommandLineControl::validParams()
{
  InputParameters params = Control::validParams();
  params += SamplerInterface::validParams();
  params.addClassDescription("Control for modifying the command line arguments of MultiApps.");

  // Set and suppress the 'execute_on' flag, it doesn't work with any other flag
  params.set<ExecFlagEnum>("execute_on") = {EXEC_PRE_MULTIAPP_SETUP};
  params.suppressParameter<ExecFlagEnum>("execute_on");

  params.addRequiredParam<MultiAppName>("multi_app", "The name of the MultiApp to control.");
  params.addParam<SamplerName>(
      "sampler",
      "The Sampler object to utilize for altering the command line options of the MultiApp.");
  params.addParam<std::vector<std::string>>(
      "param_names", "The names of the command line parameters to set via the sampled data.");

  return params;
}

MultiAppCommandLineControl::MultiAppCommandLineControl(const InputParameters & parameters)
  : Control(parameters),
    SamplerInterface(this),
    _multi_app(_fe_problem.getMultiApp(getParam<MultiAppName>("multi_app"))),
    _sampler(SamplerInterface::getSampler("sampler")),
    _param_names(getParam<std::vector<std::string>>("param_names"))
{
  if (!_sampler.getParam<ExecFlagEnum>("execute_on").contains(EXEC_PRE_MULTIAPP_SETUP))
    _sampler.paramError(
        "execute_on",
        "The sampler object, '",
        _sampler.name(),
        "', is being used by the '",
        name(),
        "' object, thus the 'execute_on' of the sampler must include 'PRE_MULTIAPP_SETUP'.");

  bool batch_reset = _multi_app->isParamValid("mode") &&
                     (_multi_app->getParam<MooseEnum>("mode") == "batch-reset");
  bool batch_restore = _multi_app->isParamValid("mode") &&
                       (_multi_app->getParam<MooseEnum>("mode") == "batch-restore");
  if (batch_reset)
    ; // Do not perform the App count test, because in batch mode there is only one

  else if (batch_restore)
    _multi_app->paramError(
        "mode",
        "The MultiApp object, '",
        _multi_app->name(),
        "', supplied to the '",
        name(),
        "' object is setup to run in 'batch-restore' mode, when using this mode command line "
        "arguments cannot be modified; batch-reset mode should be used instead.");

  else if (_multi_app->numGlobalApps() != _sampler.getNumberOfRows())
    mooseError("The number of sub apps (",
               _multi_app->numGlobalApps(),
               ") created by MultiApp object '",
               _multi_app->name(),
               "' must be equal to the number for rows (",
               _sampler.getNumberOfRows(),
               ") for the '",
               _sampler.name(),
               "' Sampler object.");
}

void
MultiAppCommandLineControl::initialSetup()
{
  // Do not put anything here, this method is being called after execute because the execute_on
  // is set to PRE_MULTIAPP_SETUP for this class. It won't work any other way.
}

void
MultiAppCommandLineControl::execute()
{
  std::vector<std::string> cli_args;

  if (_sampler.getNumberOfCols() != _param_names.size())
    paramError("param_names",
               "The number of columns (",
               _sampler.getNumberOfCols(),
               ") must match the number of parameters (",
               _param_names.size(),
               ").");

  // For SamplerFullSolveMultiApp, to avoid storing duplicated param_names for each sampler, we
  // store only param_names once in "cli_args". For other MultApp, we store the full information
  // of params_names and values for each sampler.

  if (std::dynamic_pointer_cast<SamplerFullSolveMultiApp>(_multi_app) == nullptr)
  {
    for (dof_id_type row = _sampler.getLocalRowBegin(); row < _sampler.getLocalRowEnd(); ++row)
    {
      std::vector<Real> data = _sampler.getNextLocalRow();
      std::ostringstream oss;
      for (std::size_t col = 0; col < data.size(); ++col)
      {
        if (col > 0)
          oss << ";";
        oss << _param_names[col] << "=" << Moose::stringify(data[col]);
      }

      cli_args.push_back(oss.str());
    }
  }
  else
  {
    std::ostringstream oss;
    for (dof_id_type col = 0; col < _sampler.getNumberOfCols(); ++col)
    {
      if (col > 0)
        oss << ";";
      oss << _param_names[col];
    }

    cli_args.push_back(oss.str());
  }

  setControllableValueByName<std::vector<std::string>>(
      "MultiApp", _multi_app->name(), "cli_args", cli_args);
}
