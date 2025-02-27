
#if !defined(KRATOS_VECTOR_FIELD_H)
#define KRATOS_VECTOR_FIELD_H

// /* External includes */

// System includes

// Project includes
#include "includes/variables.h"

/* System includes */
#include <limits>
#include <iostream>
#include <iomanip>

/* External includes */
#ifdef _OPENMP
#include <omp.h>
#endif

#include "boost/smart_ptr.hpp"

/* Project includes */
#include "includes/define.h"
#include "utilities/openmp_utils.h"
#include "real_functions.h"


namespace Kratos
{
template<std::size_t TDim>
class VectorField
{
public:

KRATOS_CLASS_POINTER_DEFINITION(VectorField);

/// Default constructor.

VectorField():
mUx(LinearFunction(0, 0)), mUy(LinearFunction(0, 0)), mUz(LinearFunction(0, 0)){}

VectorField(RealFunction& u_x, RealFunction& u_y, RealFunction& u_z):
mUx(u_x), mUy(u_y), mUz(u_z){}
/// Destructor.

virtual ~VectorField(){}


//***************************************************************************************************************
//***************************************************************************************************************

virtual void Evaluate(const double time, const array_1d<double, 3>& coor, array_1d<double, 3>& vector){}

virtual void CalculateTimeDerivative(const double time, const array_1d<double, 3>& coor, array_1d<double, 3>& deriv){}

virtual void CalculateGradient(const double time, const array_1d<double, 3>& coor, array_1d< array_1d<double, 3>, 3>& gradient){}

virtual void CalculateDivergence(const double time, const array_1d<double, 3>& coor, double& div){}

virtual void CalculateRotational(const double time, const array_1d<double, 3>& coor, array_1d<double, 3>& rot){}

//***************************************************************************************************************
//***************************************************************************************************************

///@}
///@name Inquiry
///@{


///@}
///@name Input and output
///@{

/// Turn back information as a stemplate<class T, std::size_t dim> tring.

virtual std::string Info() const
{
    return "";
}

/// Print information about this object.

virtual void PrintInfo(std::ostream& rOStream) const
{
}

/// Print object's data.

virtual void PrintData(std::ostream& rOStream) const
{
}


///@}
///@name Friends
///@{

///@}

protected:
///@name Protected static Member r_variables
///@{


///@}
///@name Protected member r_variables
///@{ template<class T, std::size_t dim>


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

///@name Static Member r_variables
///@{


///@}
///@name Member r_variables
///@{

RealFunction mUx;
RealFunction mUy;
RealFunction mUz;

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
VectorField & operator=(VectorField const& rOther);


///@}

}; // Class VectorField

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{

} // namespace Kratos.

#endif // KRATOS_VECTOR_FIELD_H defined
