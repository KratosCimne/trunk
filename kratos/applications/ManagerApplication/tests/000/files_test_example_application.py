import os
   
   # Import Kratos
   from KratosMultiphysics import *
   
   # Import KratosUnittest
   import KratosMultiphysics.KratosUnittest as KratosUnittest
   import Kratos_Execute_Solid_Test as Execute_Test
   
   
   # This utiltiy will control the execution scope in case we need to acces files or we depend
   # on specific relative locations of the files.
   
   class controlledExecutionScope:
       def __init__(self, scope):
           self.currentPath = os.getcwd()
           self.scope = scope
   
       def __enter__(self):
           os.chdir(self.scope)
   
       def __exit__(self, type, value, traceback):
           os.chdir(self.currentPath)
   
   
   class HenryPureDiffTestFactory(KratosUnittest.TestCase):
   
       def setUp(self):
           # Within this location context:
           with controlledExecutionScope(os.path.dirname(os.path.realpath(__file__))):
               # Initialize GiD  I/O
               parameter_file = open(self.file_name + "_parameters.json", 'r')
               ProjectParameters = Parameters(parameter_file.read())
   
               # Creating the model part
               self.test = Execute_Test.Kratos_Execute_Test(ProjectParameters)
   
       def test_execution(self):
           # Within this location context:
           with controlledExecutionScope(os.path.dirname(os.path.realpath(__file__))):
               self.test.Solve()
   
       def tearDown(self):
           pass
   
   
   class HenyPressureHydrostaticTests(HenryPureDiffTestFactory):
       file_name = "henry_test/pressure_hydrostatic_test"
   
   class HenyPressureHoritTests(HenryPureDiffTestFactory):
       file_name = "henry_test/pressure_horitzontal_test"
   
   class PureDiffTests(HenryPureDiffTestFactory):
       file_name = "pure_diffusion_test/heat_point_test"
   

