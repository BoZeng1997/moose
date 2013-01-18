
[GlobalParams]
  disp_x = disp_x
  disp_y = disp_y
  disp_z = disp_z
[]

[Mesh]
  file = pressure.e
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]

  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]

  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
[] # Variables

[AuxVariables]

  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]

[] # AuxVariables

[SolidMechanics]
  [./solid]
  [../]
[]

[Contact]
  [./dummy_name]
    master = 20
    slave = 10
    penalty = 1e8
    formulation = penalty
#    model = glued
    tangential_tolerance = 1e-3
  [../]
[]

[AuxKernels]
  [./stress_yy]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_yy
    index = 1
    execute_on = timestep
  [../]
[] # AuxKernels

[BCs]
  [./left_x]
    type = DirichletBC
    variable = disp_x
    boundary = 3
    value = 0.0
  [../]

  [./bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = 1
    value = 0.0
  [../]

  [./z]
    type = DirichletBC
    variable = disp_z
    boundary = 5
    value = 0.0
  [../]

  [./Pressure]
    [./press]
      boundary = 7
      factor = 1e3
    [../]
  [../]

  [./down]
    type = PresetBC
    variable = disp_y
    boundary = 8
    value = -2e-3
  [../]

[] # BCs

[Materials]

  [./stiffStuff1]
    type = Elastic
    block = 1

    youngs_modulus = 1e6
    poissons_ratio = 0.0
  [../]
  [./stiffStuff2]
    type = Elastic
    block = 2

    youngs_modulus = 1e6
    poissons_ratio = 0.0
  [../]
[] # Materials

[Executioner]
  type = Transient
  petsc_options = '-snes_mf_operator -ksp_monitor'
  petsc_options_iname = '-pc_type -snes_type -snes_ls -snes_linesearch_type -ksp_gmres_restart'
  petsc_options_value = 'lu       ls         basic    basic                    101'

  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-6

  l_max_its = 100
  nl_max_its = 10
  dt = 1.0
  num_steps = 1
[] # Executioner

[Output]
  interval = 1
  output_initial = true
  exodus = true
  perf_log = true
[] # Output
