#importing the Kratos Library
from KratosMultiphysics import *
from KratosMultiphysics.HenryApplication import *

#Method to add all the variables to our problem: solver to mdpa object
def AddVariables(model_part):
    #model_part.AddNodalSolutionStepVariable(HEAD_LEVEL);
    model_part.AddNodalSolutionStepVariable(PRESSURE);
    model_part.AddNodalSolutionStepVariable(CONCENTRATION);
    model_part.AddNodalSolutionStepVariable(DENSITY);

    print ("HEAD_LEVEL &  CONCENTRATION variable added correctly")

#Method to add all the unkwons to our problem: solver to mdpa object
def AddDofs(model_part):
    for node in model_part.Nodes:
        #node.AddDof(HEAD_LEVEL);
        node.AddDof(PRESSURE);
        node.AddDof(CONCENTRATION);

    print ("degrees of freedom HEAD_LEVEL &  CONCENTRATION added correctly")

#Flow solver constructor, arguments: modelpart & domainSize
#Fractional solver 
class FractionalSolver1:
    #######################################################################
    def __init__(self,model_part,domain_size):
        
	self.model_part = model_part
	self.domain_size = domain_size
        
        self.time_scheme = ResidualBasedIncrementalUpdateStaticScheme()

        #definition of the solvers
	self.basic_linear_solver = SkylineLUFactorizationSolver()

        #definition of the convergence criteria
	#tolerance for the solver
        self.conv_criteria = DisplacementCriteria(1e-5,1e-15)
        
    #######################################################################
    #######################################################################
    def Initialize(self):
        #creating the solution strategy
        CalculateReactionFlag = False
        ReformDofSetAtEachStep = True ##Cambio a true para recalculo de Dof para la siguiente PDE	
        MoveMeshFlag = False
        
        import strategyBasicFlow
        self.solver = strategyBasicFlow.SolvingStrategyPython(self.model_part,self.time_scheme,self.basic_linear_solver,self.conv_criteria,CalculateReactionFlag,ReformDofSetAtEachStep,MoveMeshFlag)
	self.solver.SetEchoLevel(3)

    #######################################################################   
    def Solve(self):
          self.model_part.ProcessInfo.SetValue(FRACTIONAL_STEP, 1)
	  (self.solver).Solve()
          self.model_part.ProcessInfo.SetValue(FRACTIONAL_STEP, 2)
          (self.solver).Solve()
    ####################################################################### 

    def SetEchoLevel(self,level):
        (self.solver).SetEchoLevel(level)


