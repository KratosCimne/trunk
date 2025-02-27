//   
//   Project Name:        Kratos       
//   Last Modified by:    $Author:  $
//   Date:                $Date:  $
//   Revision:            $Revision: 1.2 $
//
//


#if !defined(KRATOS_HENRY_APPLICATION_H_INCLUDED )
#define  KRATOS_HENRY_APPLICATION_H_INCLUDED



// System includes
#include <string>
#include <iostream> 


// External includes 


// Project includes
#include "includes/define.h"
#include "includes/kratos_application.h"

//#include "porous_media_application_variables.h"


#include "custom_elements/flowpressuretrans_2d.h" 

#include "custom_conditions/sinksourcepressure.h"

#include "custom_conditions/massflow.h"


#include "includes/variables.h"
#include "includes/condition.h"         //we'll also need conditions for the point heat loads

#include "includes/ublas_interface.h"


namespace Kratos
{

	///@name Kratos Globals
	///@{ 

	// Variables definition 

        //General transport & flow
        KRATOS_DEFINE_VARIABLE(int, IS_FLOW_STATIONARY)
        KRATOS_DEFINE_VARIABLE(int, IS_BUOYANCY)
        KRATOS_DEFINE_VARIABLE(int, IS_TRANSIENT)
        KRATOS_DEFINE_VARIABLE(int, IS_TRANSPORT_STATIONARY)
        KRATOS_DEFINE_VARIABLE(double, HEAD_LEVEL)
        KRATOS_DEFINE_VARIABLE(double, SPECIFIC_STORAGE) //Eliminar de flowpressTransElem (cambiar por _elem)
        KRATOS_DEFINE_VARIABLE(double, DENSITY_ELEM)
        KRATOS_DEFINE_VARIABLE(Vector, DENSITY_ELEM_TARGET)
        
 
        KRATOS_DEFINE_VARIABLE(double, PERMEABILITY) //Eliminar de flowpressTransElem (cambiar por _elem)
        KRATOS_DEFINE_VARIABLE(double, DIFFUSION_COEFFICIENT) //Eliminar de flowpressTransElem (cambiar por _elem)
        

        //Conditions flow & transport
        KRATOS_DEFINE_VARIABLE(double, LEAKAGE_COEFFICIENT)
        KRATOS_DEFINE_VARIABLE(double, LEVEL)
        KRATOS_DEFINE_VARIABLE(double, PRESCRIBED_VALUE)
        KRATOS_DEFINE_VARIABLE(double, SINK_SOURCE_PRESS)
        KRATOS_DEFINE_VARIABLE(double, SINK_SOURCE)
        

        //Strategy flow & transport
        KRATOS_DEFINE_VARIABLE(double, DARCY_FLOW_X)
        KRATOS_DEFINE_VARIABLE(Vector, DARCY_FLOW_X_TARGET)
        KRATOS_DEFINE_VARIABLE(double, CONCENTRATION)
        KRATOS_DEFINE_VARIABLE(double, PRESSURE_OLD_ITT)
        KRATOS_DEFINE_VARIABLE(double, PRESCRIBED_VALUE)
        KRATOS_DEFINE_VARIABLE(double, DARCY_FLOW_Y)
        KRATOS_DEFINE_VARIABLE(double, STORAGE_BALANCE)
        KRATOS_DEFINE_VARIABLE(double, DARCY_FLOW_BALANCE) 
        KRATOS_DEFINE_VARIABLE(double, SINKSOURCE_BALANCE)
        KRATOS_DEFINE_VARIABLE(double, CONCENTRATION_OLD_IT)
        
        
        /*//Strategy multiphase
        //KRATOS_DEFINE_VARIABLE(double, ENTHALPY_WET_NODE)
        //KRATOS_DEFINE_VARIABLE(double, ENTHALPY_NON_NODE)
        
        KRATOS_DEFINE_VARIABLE(double, FIRST_SV_OLD_IT)
        KRATOS_DEFINE_VARIABLE(double, SECOND_SV_OLD_IT)
        
        KRATOS_DEFINE_VARIABLE(double, MAXDIFF_SV_1)
        KRATOS_DEFINE_VARIABLE(double, MAXDIFF_SV_2)
        KRATOS_DEFINE_VARIABLE(double, MAXITER_SV_1)
        KRATOS_DEFINE_VARIABLE(double, MAXITER_SV_2)
        KRATOS_DEFINE_VARIABLE(double, MAXNORMREL_NR)
        KRATOS_DEFINE_VARIABLE(double, MAXITER_NR)
        
        KRATOS_DEFINE_VARIABLE(int, TIMES_TO_PRINT)
        
        KRATOS_DEFINE_VARIABLE(double, STORAGE_NON_BALANCE)
        KRATOS_DEFINE_VARIABLE(double, STORAGE_NON_COMPRES_BALANCE)
        KRATOS_DEFINE_VARIABLE(double, FRACTIONAL_FLOW_BALANCE)
        KRATOS_DEFINE_VARIABLE(double, MOVEMENT_BETWEEN_PHASES_BALANCE)
        KRATOS_DEFINE_VARIABLE(double, SINKSOURCE_BALANCE_NON)*/
        
        // sinksource.cpp condition. Generalization of input (testing)
        /*KRATOS_DEFINE_VARIABLE(string, SV)
        KRATOS_DEFINE_VARIABLE(string, SV_X)
        KRATOS_DEFINE_VARIABLE(string, SV_Y)
        KRATOS_DEFINE_VARIABLE(string, SV_Z)*/
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

	/// Short class definition.
	/** Detail class definition.
	*/
	class KratosHenryApplication : public KratosApplication
	{
	public:
		///@name Type Definitions
		///@{
		

		/// Pointer definition of KratosHenryApplication
		KRATOS_CLASS_POINTER_DEFINITION(KratosHenryApplication);

		///@}
		///@name Life Cycle 
		///@{ 

		/// Default constructor.
		KratosHenryApplication();

		/// Destructor.
		virtual ~KratosHenryApplication(){}


		///@}
		///@name Operators 
		///@{


		///@}
		///@name Operations
		///@{

		virtual void Register();



		///@}
		///@name Access
		///@{ 


		///@}
		///@name Inquiry
		///@{


		///@}      
		///@name Input and output
		///@{

		/// Turn back information as a string.
		virtual std::string Info() const
		{
			return "KratosHenryApplication";
		}

		/// Print information about this object.
		virtual void PrintInfo(std::ostream& rOStream) const
		{
			rOStream << Info();
			PrintData(rOStream);
		}

		///// Print object's data.
      virtual void PrintData(std::ostream& rOStream) const
      {
      	KRATOS_WATCH("in my application");
      	KRATOS_WATCH(KratosComponents<VariableData>::GetComponents().size() );
		rOStream << "Variables:" << std::endl;
		KratosComponents<VariableData>().PrintData(rOStream);
		rOStream << std::endl;
		rOStream << "Elements:" << std::endl;
		KratosComponents<Element>().PrintData(rOStream);
		rOStream << std::endl;
		rOStream << "Conditions:" << std::endl;
		KratosComponents<Condition>().PrintData(rOStream);
      }


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

               	//const Flow2D  mFlow2D; //and here is our element.
                //const FlowTrans2D  mFlowTrans2D; //and here is our element.
                const FlowPressureTrans2D  mFlowPressureTrans2D; //and here is our element.

		//       static const ApplicationCondition  msApplicationCondition; 

		///@} 
		///@name Member Variables 
		///@{ 
// 		const Elem2D   mElem2D; 
// 		const Elem3D   mElem3D; 

                const SinkSourcePressure mPointSinkSourcePressure;
                const SinkSourcePressure mLineSinkSourcePressure;
                const MassFlow mMassFlow;
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

		/// Assignment operator.
		KratosHenryApplication& operator=(KratosHenryApplication const& rOther);

		/// Copy constructor.
		KratosHenryApplication(KratosHenryApplication const& rOther);


		///@}    

	}; // Class KratosHenryApplication 

	///@} 


	///@name Type Definitions       
	///@{ 


	///@} 
	///@name Input and output 
	///@{ 

	///@} 


}  // namespace Kratos.

#endif // KRATOS_HENRY_APPLICATION_H_INCLUDED  defined 


