/*
==============================================================================
KratosHenryApplication 
A library based on:
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi, Janosch Stascheit, Felix Nagel 
pooyan@cimne.upc.edu 
rrossi@cimne.upc.edu
janosch.stascheit@rub.de
nagel@sd.rub.de
- CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain
- Ruhr-University Bochum, Institute for Structural Mechanics, Germany


Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

The  above  copyright  notice  and  this permission  notice  shall  be
included in all copies or substantial portions of the Software.

THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==============================================================================
*/
 
//   
//   Project Name:        Kratos       
//   Last modified by:    $Author:  $
//   Date:                $Date: $
//   Revision:            $Revision: 1.3 $
//
//

// System includes 

#if defined(KRATOS_PYTHON)
// External includes 
#include <boost/python.hpp>


// Project includes 
#include "includes/define.h"
#include "henry_application.h"
#include "custom_python/add_custom_strategies_to_python.h"
#include "custom_python/add_custom_utilities_to_python.h"
#include "custom_python/add_tests_to_python.h"

 
namespace Kratos
{

namespace Python
{

  using namespace boost::python;


  
  BOOST_PYTHON_MODULE(KratosHenryApplication)
  {

	  class_<KratosHenryApplication, 
			  KratosHenryApplication::Pointer, 
			  bases<KratosApplication>, boost::noncopyable >("KratosHenryApplication")
			;

	AddCustomStrategiesToPython();
	AddCustomUtilitiesToPython();
	AddTestsToPython();
        
        	//registering variables in python    
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(IS_FLOW_STATIONARY);
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(IS_TRANSPORT_STATIONARY);
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(IS_BUOYANCY);
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( IS_TRANSIENT );  
        
        
        
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(HEAD_LEVEL);
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(SPECIFIC_STORAGE);
        
        
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( DENSITY_ELEM );
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( DENSITY_ELEM_TARGET );
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( CONCENTRATION); 
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(PERMEABILITY);
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( DIFFUSION_COEFFICIENT );
        
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( CONCENTRATION_OLD_IT );
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(PRESSURE_OLD_ITT );
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(PRESCRIBED_VALUE );
        
        
  
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(LEAKAGE_COEFFICIENT);
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(LEVEL);
        
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(SINK_SOURCE);
        KRATOS_REGISTER_IN_PYTHON_VARIABLE(SINK_SOURCE_PRESS);
        
        
        
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( DARCY_FLOW_X );
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( DARCY_FLOW_X_TARGET );
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( DARCY_FLOW_Y );

                              
        

        
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( STORAGE_BALANCE ); 
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( DARCY_FLOW_BALANCE );
        KRATOS_REGISTER_IN_PYTHON_VARIABLE( SINKSOURCE_BALANCE );


        


	//registering variables in python
//	KRATOS_REGISTER_IN_PYTHON_VARIABLE(NODAL_AREA);


  }
  
  
}  // namespace Python.
  
}  // namespace Kratos.

#endif // KRATOS_PYTHON defined
