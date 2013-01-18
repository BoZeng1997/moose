#
# Internal Volume Test
#
# This test is designed to compute the internal volume of a space considering
#   an embedded volume inside.
#
# The mesh is composed of one block (1) with an interior cavity of volume 8.
#   Block 2 sits in the cavity and has a volume of 1.  Thus, the total volume
#   is 7.
#

[Mesh]#Comment
  file = internal_volume_hex20.e
  displacements = 'disp_x disp_y disp_z'
[]

[Functions]
  [./step]
    type = PiecewiseLinear
    x = '0. 1. 2. 3.'
    y = '0. 0. 1e-2 0.'
    scale_factor = 0.5
  [../]
[]

[Variables]

#  active = 'disp_x disp_y disp_z'

  [./disp_x]
    order = SECOND
    family = LAGRANGE
  [../]

  [./disp_y]
    order = SECOND
    family = LAGRANGE
  [../]

  [./disp_z]
    order = SECOND
    family = LAGRANGE
  [../]

[]

[SolidMechanics]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
  [../]
[]

[BCs]

  active = 'no_x no_y prescribed_z'

  [./no_x]
    type = DirichletBC
    variable = disp_x
    boundary = 100
    value = 0.0
  [../]

  [./no_y]
    type = DirichletBC
    variable = disp_y
    boundary = 100
    value = 0.0
  [../]

  [./prescribed_z]
    type = FunctionPresetBC
    variable = disp_z
    boundary = 100
    function = step
  [../]
[]

[Materials]
  active = 'stiffStuff stiffStuff2'

  [./stiffStuff]
    type = LinearIsotropicMaterial
    block = 1

    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z

    youngs_modulus = 1e6
    poissons_ratio = 0.3
    thermal_expansion = 1e-5
    t_ref = 400.
  [../]

  [./stiffStuff2]
    type = LinearIsotropicMaterial
    block = 2

    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z

    youngs_modulus = 1e6
    poissons_ratio = 0.3
    thermal_expansion = 1e-5
    t_ref = 400.
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  []
[]

[Executioner]

  type = Transient
  petsc_options = '-snes_mf_operator -ksp_monitor'
  petsc_options_iname = '-pc_type -snes_type -snes_ls -snes_linesearch_type -ksp_gmres_restart'
  petsc_options_value = 'lu       ls         basic    basic                    101'

  nl_abs_tol = 1e-10

  l_max_its = 20

  start_time = 0.0
  dt = 1.0
  #num_steps = 3
  end_time = 3.0

  [./Quadrature]
    order = THIRD
  [../]

[]

[Postprocessors]
  [./internalVolume]
    type = InternalVolume
    boundary = 100
    variable = disp_x
  [../]

  [./dispZ]
    type = ElementAverageValue
    block = '1 2'
    variable = disp_z
  [../]
[]

[Output]
  file_base = out
  interval = 1
  output_initial = true
  exodus = true
  postprocessor_csv = true
  perf_log = true
[]
