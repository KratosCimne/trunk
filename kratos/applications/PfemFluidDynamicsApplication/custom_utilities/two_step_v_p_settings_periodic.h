//
//   Project Name:        KratosPFEMFluidDynamicsApplication $
//   Last modified by:    $Author:                   AFranci $
//   Date:                $Date:                January 2016 $
//   Revision:            $Revision:                     0.0 $
//
//

#ifndef KRATOS_TWO_STEP_V_P_SETTINGS_PERIODIC_H
#define KRATOS_TWO_STEP_V_P_SETTINGS_PERIODIC_H

// System includes
#include <string>
#include <iostream>

// External includes

// Project includes
#include "includes/define.h"
#include "solving_strategies/builder_and_solvers/builder_and_solver.h"
#include "solving_strategies/convergencecriterias/convergence_criteria.h"
#include "solving_strategies/convergencecriterias/residual_criteria.h"
#include "solving_strategies/schemes/scheme.h"
#include "solving_strategies/schemes/residualbased_incrementalupdate_static_scheme.h"
#include "solving_strategies/schemes/residualbased_incrementalupdate_static_scheme_slip.h"
#include "solving_strategies/strategies/solving_strategy.h"
#include "solving_strategies/strategies/residualbased_linear_strategy.h"
#include "processes/process.h"

// Application includes
#include "custom_processes/spalart_allmaras_turbulence_model.h" 
#include "custom_utilities/solver_settings.h"
#include "custom_strategies/builders_and_solvers/residualbased_block_builder_and_solver_periodic.h"


namespace Kratos
{
///@addtogroup PfemFluidDynamicsApplication
///@{

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/// Helper class to define solution strategies for FS_Strategy.
template< class TSparseSpace,
          class TDenseSpace,
          class TLinearSolver
          >
class TwoStepVPSettingsPeriodic: public TwoStepVPSolverSettings<TSparseSpace,TDenseSpace,TLinearSolver>
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of TwoStepVPSettingsPeriodic
    KRATOS_CLASS_POINTER_DEFINITION(TwoStepVPSettingsPeriodic);

    typedef TwoStepVPSolverSettings<TSparseSpace,TDenseSpace,TLinearSolver> BaseType;
    typedef typename BaseType::StrategyType StrategyType;
    typedef typename BaseType::StrategyPointerType StrategyPointerType;
    typedef typename BaseType::ProcessPointerType ProcessPointerType;

    typedef typename BaseType::StrategyLabel StrategyLabel;
    typedef typename BaseType::TurbulenceModelLabel TurbulenceModelLabel;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Constructor.
    TwoStepVPSettingsPeriodic(ModelPart& rModelPart,
                   const unsigned int ThisDomainSize,
                   const unsigned int ThisTimeOrder,
                   const bool UseSlip,
                   const bool MoveMeshFlag,
		   const bool ReformDofSet,
		   const Kratos::Variable<int>& rPeriodicVar):
    BaseType(rModelPart,ThisDomainSize,ThisTimeOrder,UseSlip,MoveMeshFlag,ReformDofSet),
    mrPeriodicVar(rPeriodicVar)
    {}

    /// Destructor.
    virtual ~TwoStepVPSettingsPeriodic(){}

    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    ///@}
    ///@name Access
    ///@{

    virtual void SetStrategy(StrategyLabel const& rStrategyLabel,
                             typename TLinearSolver::Pointer pLinearSolver,
                             const double Tolerance,
                             const unsigned int MaxIter)
    {
        KRATOS_TRY;

        // pointer types for solution strategy construcion
        typedef typename Scheme< TSparseSpace, TDenseSpace >::Pointer SchemePointerType;
        typedef typename ConvergenceCriteria< TSparseSpace, TDenseSpace >::Pointer ConvergenceCriteriaPointerType;
        typedef typename BuilderAndSolver<TSparseSpace, TDenseSpace, TLinearSolver>::Pointer BuilderSolverTypePointer;

        // Default, fixed flags
        bool CalculateReactions = false;
        bool CalculateNormDxFlag = true;

        ModelPart& rModelPart = BaseType::GetModelPart();
        bool UseSlip = BaseType::UseSlipConditions();
        // Modification of the DofSet is managed by the two step v p strategy, not the auxiliary velocity and pressure strategies.
        bool ReformDofSet = false; //BaseType::GetReformDofSet();
        unsigned int EchoLevel = BaseType::GetEchoLevel();
        unsigned int StrategyEchoLevel = (EchoLevel > 0) ? (EchoLevel-1) : 0;

        if ( rStrategyLabel == BaseType::Velocity )
        {
            // Velocity Builder and Solver
	  BuilderSolverTypePointer pBuildAndSolver = BuilderSolverTypePointer(new ResidualBasedBlockBuilderAndSolverPeriodic<TSparseSpace, TDenseSpace, TLinearSolver> (pLinearSolver, mrPeriodicVar));

            SchemePointerType pScheme;
            //initializing fractional velocity solution step
            if (UseSlip)
            {
                unsigned int DomainSize = BaseType::GetDomainSize();
                SchemePointerType Temp = SchemePointerType(new ResidualBasedIncrementalUpdateStaticSchemeSlip< TSparseSpace, TDenseSpace > (DomainSize,DomainSize));
                pScheme.swap(Temp);
            }
            else
            {
                SchemePointerType Temp = SchemePointerType(new ResidualBasedIncrementalUpdateStaticScheme< TSparseSpace, TDenseSpace > ());
                pScheme.swap(Temp);
            }

            // Strategy
            BaseType::mStrategies[rStrategyLabel] = StrategyPointerType(new ResidualBasedLinearStrategy<TSparseSpace, TDenseSpace, TLinearSolver >
                                                                        (rModelPart, pScheme, pLinearSolver, pBuildAndSolver, CalculateReactions, ReformDofSet, CalculateNormDxFlag));

        }
        else if ( rStrategyLabel == BaseType::Pressure )
        {
            // Pressure Builder and Solver
	  BuilderSolverTypePointer pBuildAndSolver = BuilderSolverTypePointer(new ResidualBasedBlockBuilderAndSolverPeriodic<TSparseSpace, TDenseSpace, TLinearSolver> (pLinearSolver, mrPeriodicVar));

            SchemePointerType pScheme = SchemePointerType(new ResidualBasedIncrementalUpdateStaticScheme< TSparseSpace, TDenseSpace > ());

            // Strategy
            BaseType::mStrategies[rStrategyLabel] = StrategyPointerType(new ResidualBasedLinearStrategy<TSparseSpace, TDenseSpace, TLinearSolver >
                                                                        (rModelPart, pScheme, pLinearSolver, pBuildAndSolver, CalculateReactions, ReformDofSet, CalculateNormDxFlag));
        }
        else
        {
            KRATOS_THROW_ERROR(std::runtime_error,"Error in TwoStepVPSettingsPeriodic: Unknown strategy label.","");
        }

        BaseType::mTolerances[rStrategyLabel] = Tolerance;

        BaseType::mMaxIter[rStrategyLabel] = MaxIter;

        BaseType::mStrategies[rStrategyLabel]->SetEchoLevel(StrategyEchoLevel);

        KRATOS_CATCH("");
    }

    virtual void SetTurbulenceModel(TurbulenceModelLabel const& rTurbulenceModel,
                                    typename TLinearSolver::Pointer pLinearSolver,
                                    const double Tolerance,
                                    const unsigned int MaxIter)
    {
        KRATOS_TRY;

        BaseType::mHaveTurbulenceModel = true;

        ModelPart& rModelPart = BaseType::GetModelPart();
        unsigned int DomainSize = BaseType::GetDomainSize();
        unsigned int TimeOrder = BaseType::GetTimeOrder();
        bool ReformDofSet = BaseType::GetReformDofSet();

        if (rTurbulenceModel == BaseType::SpalartAllmaras)
        {
            BaseType::mpTurbulenceModel = ProcessPointerType( new SpalartAllmarasTurbulenceModel<TSparseSpace,TDenseSpace,TLinearSolver>
                                                              (rModelPart,pLinearSolver,DomainSize,Tolerance,MaxIter,ReformDofSet,TimeOrder));
            /* KRATOS_THROW_ERROR(std::runtime_error,"Error in TwoStepVPSettingsPeriodic: SpalartAllmaras not included",""); */

        }
        else
        {
            KRATOS_THROW_ERROR(std::runtime_error,"Error in TwoStepVPSettingsPeriodic: Unknown turbulence model label.","");
        }

        KRATOS_CATCH("");
    }

    virtual void SetTurbulenceModel(ProcessPointerType pTurbulenceModel)
    {
        BaseType::SetTurbulenceModel(pTurbulenceModel);
    }

    ///@}
    ///@name Inquiry
    ///@{

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    virtual std::string Info() const
    {
        std::stringstream buffer;
        buffer << "TwoStepVPSettingsPeriodic" ;
        return buffer.str();
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const {rOStream << "TwoStepVPSettingsPeriodic";}

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const {}


    ///@}
    ///@name Friends
    ///@{


    ///@}

protected:
    ///@name Protected static Member Variables
    ///@{


    ///@}
    ///@name Protected member Variables
    ///@{


    ///@}
    ///@name Protected Operators
    ///@{


    ///@}
    ///@name Protected Operations
    ///@{


    ///@}
    ///@name Protected  Access
    ///@{


    ///@}
    ///@name Protected Inquiry
    ///@{


    ///@}
    ///@name Protected LifeCycle
    ///@{


    ///@}

private:
    ///@name Static Member Variables
    ///@{


    ///@}
    ///@name Member Variables
    ///@{
    const Kratos::Variable<int>& mrPeriodicVar;

    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{


    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    /// Default constructor.
    TwoStepVPSettingsPeriodic(){}

    /// Assignment operator.
    TwoStepVPSettingsPeriodic& operator=(TwoStepVPSettingsPeriodic const& rOther){}

    /// Copy constructor.
    TwoStepVPSettingsPeriodic(TwoStepVPSettingsPeriodic const& rOther){}


    ///@}

}; // Class TwoStepVPSettingsPeriodic

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
template< class TDenseSpace, class TSparseSpace, class TLinearSolver >
inline std::istream& operator >> (std::istream& rIStream,
                                  TwoStepVPSettingsPeriodic<TSparseSpace,TDenseSpace,TLinearSolver>& rThis)
{
    return rIStream;
}

/// output stream function
template< class TDenseSpace, class TSparseSpace, class TLinearSolver >
inline std::ostream& operator << (std::ostream& rOStream,
                                  const TwoStepVPSettingsPeriodic<TSparseSpace,TDenseSpace,TLinearSolver>& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}
///@}

///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_TWO_STEP_V_P_SETTINGS_PERIODIC_H
