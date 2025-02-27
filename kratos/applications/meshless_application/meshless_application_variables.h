/*
==============================================================================
KratosFluidDynamicsApplication
A library based on:
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis

Copyright 2010
Pooyan Dadvand, Riccardo Rossi
pooyan@cimne.upc.edu
rrossi@cimne.upc.edu
- CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain


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
//   Last Modified by:    $Author: jcotela $
//   Date:                $Date: 2010-11-11 $
//   Revision:            $Revision: 1.0 $
//
//


#if !defined(KRATOS_MESHLESS_APPLICATION_VARIABLES_H_INCLUDED )
#define  KRATOS_MESHLESS_APPLICATION_VARIABLES_H_INCLUDED

// System includes

// External includes


// Project includes
#include "includes/define.h"
#include "includes/kratos_application.h"
#include "includes/variables.h"

namespace Kratos
{
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(PRESSURE_ACC);
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(VISCOUS_ACC);
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(BODYFORCE_ACC);
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(BOUNDARY_ACC);
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(TEMP_POS);
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(TEMP_VEL);
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(TEMP_RHS);
KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(TEMP_DISP);

KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(XSPH_VELOCITY);

KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(OLD_VEL);

KRATOS_DEFINE_VARIABLE(double, EFFECTIVE_RADIUS );
KRATOS_DEFINE_VARIABLE(double, DENSITY_NORM_PARAM );
KRATOS_DEFINE_VARIABLE(double, DIV_OF_VEL );

KRATOS_DEFINE_VARIABLE(double, OLD_DENSITY );

KRATOS_DEFINE_VARIABLE(double, IS_WET );
KRATOS_DEFINE_VARIABLE(double, DENS_VARIATION );
KRATOS_DEFINE_VARIABLE(double, DENS_DIFF );
KRATOS_DEFINE_VARIABLE(double, INI_PRESSURE );
KRATOS_DEFINE_VARIABLE(double, OUT_OF_SYSTEM );


KRATOS_DEFINE_VARIABLE(double, VER_WALL_LEFT);
KRATOS_DEFINE_VARIABLE(double, VER_WALL_RIGHT);
KRATOS_DEFINE_VARIABLE(double, HOR_WALL_BOTTOM);



KRATOS_DEFINE_VARIABLE(double, DUMMY_NORMALIZE_RHS);
KRATOS_DEFINE_VARIABLE(double, DUMMY_APPLY_XSPH);
KRATOS_DEFINE_VARIABLE(double, DUMMY_BOUNDARY_PRESSURES);
KRATOS_DEFINE_VARIABLE(double, DUMMY_CATCH_FREESURFACE);
KRATOS_DEFINE_VARIABLE(double, DUMMY_INTERMEDIATE_RHS);
KRATOS_DEFINE_VARIABLE(double, DELTA_TIME_ISPH);

}

#endif	/* KRATOS_MESHLESS_APPLICATION_VARIABLES_H_INCLUDED */

