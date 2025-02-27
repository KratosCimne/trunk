from __future__ import print_function, absolute_import, division #makes KratosMultiphysics backward compatible with python 2.6 and 2.7
import sys

kratos_benchmarking_path = '../../../../benchmarking'
sys.path.append(kratos_benchmarking_path)

import benchmarking

print("Building reference data for VMS2D test...")
benchmarking.BuildReferenceData("script_elemtest.py", "vms2d_test_ref.txt")
