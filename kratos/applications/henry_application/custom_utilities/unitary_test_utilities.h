/* *********************************************************
 *
 *   Last Modified by:    $Author: VictorBez $
 *   Date:                $Date: 2015        $
 *   Revision:            $Revision: 1.0     $
 *
 * ***********************************************************/


#if !defined(KRATOS_UNITARY_TEST_UTILITIES)
#define  KRATOS_UNITARY_TEST_UTILITIES


/* System includes */


/* External includes */
#include "boost/smart_ptr.hpp"


/* Project includes */
#include "includes/define.h"
#include "includes/model_part.h"
#include "utilities/geometry_utilities.h"
#include <string>
#include <stdio.h>
//Inheritance
#include "solving_strategies/strategies/solving_strategy.h"
#include "custom_elements/flowpressuretrans_2d.h"
#include "geometries/triangle_2d_3.h"


//Application
#include "henry_application.h"

using namespace std;

namespace Kratos
{
    
class UnitaryTestUtilities
{
public:
    
    //Doc-> constructor
    UnitaryTestUtilities(ModelPart& model_part,unsigned int aDomainSize = 2): mr_model_part(model_part)
    {   

    }

   
    //Doc-> Destructor
    ~UnitaryTestUtilities(){}
 


    void UnitaryTest()
    {
         FlowPressureTrans2D  elementFlowPressureTrans2D; 
         ModelPart::ElementsContainerType::iterator ielembegin = mr_model_part.ElementsBegin();
         for (int i=0; i < mr_model_part.Elements().size(); i++)
         {
              elementFlowPressureTrans2D.CalculateDensityElement();
              this->mFlowPressureTrans2D.CalculateDensityElement();//Solo podria acceder con un model part, como : ielembegin->CalculateUnitaryFunctions(); (siendo "CalculateUnitaryFunctions()" una fucción de elemets.h" , no???? porque solo entrenado como model part tengo las variables y los dof del sistema, no?
              KRATOS_WATCH("UnitaryTestUtilities")
         }
         
    }


    
protected:
    


private:    

     FlowPressureTrans2D  mFlowPressureTrans2D;
     ModelPart& mr_model_part;
}; /* Class UnitaryTestUtilities */

} /* namespace Kratos.*/

#endif /* KRATOS_UNITARY_TEST_UTILITIES  defined */

