from __future__ import print_function, absolute_import, division #makes KratosMultiphysics backward compatible with python 2.6 and 2.7
# -*- coding: utf-8 -*-
# importing the Kratos Library
from KratosMultiphysics import *
from KratosMultiphysics.HenryApplication import *
# Check that KratosMultiphysics was imported in the main script
CheckForPreviousImport()

#from KratosMultiphysics.KratosUnittest import*





class UnitaryTestHenryECU:

    def __init__(self, model_part, domain_size):

        self.model_part = model_part
        self.domain_size = domain_size
        print("finished initialization of the UnitaryTestHenryECU")
    
    #def Initialize(self):

        #self.domain_size = int(self.domain_size)


        #self.solver = FractionalIterativeStrategy(
            #self.model_part,
            #self.domain_size)

        #print("finished initialization of the UnitaryTestHenryECU")


    def UnitaryTest(self):
        (self.solver).UnitaryTest()


    def Clear(self):
        (self.solver).Clear()

