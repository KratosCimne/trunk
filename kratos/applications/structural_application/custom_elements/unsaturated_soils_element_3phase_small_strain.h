/*
==============================================================================
KratosStructuralApplication
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
//   Last Modified by:    $Author: nagel $
//   Date:                $Date: 2009-03-09 11:36:21 $
//   Revision:            $Revision: 1.4 $
//
//


#if !defined(KRATOS_UNSATURATED_SOILS_ELEMENT_3PHASE_SMALL_STRAIN_INCLUDED )
#define  KRATOS_UNSATURATED_SOILS_ELEMENT_3PHASE_SMALL_STRAIN_INCLUDED



// System includes


// External includes
#include "boost/smart_ptr.hpp"


// Project includes
#include "includes/define.h"
#include "includes/serializer.h"
#include "includes/element.h"
#include "includes/ublas_interface.h"
#include "includes/variables.h"
#include "includes/constitutive_law.h"

namespace Kratos
{

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

/// Short class definition.
/** Detail class definition.
 */

class UnsaturatedSoilsElement_3phase_SmallStrain
    : public Element
{

public:
    ///@name Type Definitions
    ///@{
    typedef GeometryData::IntegrationMethod IntegrationMethod;

    typedef ConstitutiveLaw ConstitutiveLawType;

    typedef ConstitutiveLawType::Pointer ConstitutiveLawPointerType;
    /// Counted pointer of UnsaturatedSoilsElement_3phase_SmallStrain
    KRATOS_CLASS_POINTER_DEFINITION( UnsaturatedSoilsElement_3phase_SmallStrain );

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    UnsaturatedSoilsElement_3phase_SmallStrain( IndexType NewId, GeometryType::Pointer pGeometry );
    UnsaturatedSoilsElement_3phase_SmallStrain( IndexType NewId, GeometryType::Pointer pGeometry,  PropertiesType::Pointer pProperties );

    /// Destructor.
    virtual ~UnsaturatedSoilsElement_3phase_SmallStrain();


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{
    IntegrationMethod GetIntegrationMethod() const;

    Element::Pointer Create( IndexType NewId, NodesArrayType const& ThisNodes,  PropertiesType::Pointer pProperties ) const;

    void Initialize();

    void InitializeSolutionStep( ProcessInfo& CurrentProcessInfo );

    void CalculateLocalSystem( MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo );

    void CalculateRightHandSide( VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo );

    //virtual void CalculateLeftHandSide(MatrixType& rLeftHandSideMatrix, ProcessInfo& rCurrentProcessInfo);

    void EquationIdVector( EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo );

    void GetDofList( DofsVectorType& ElementalDofList, ProcessInfo& CurrentProcessInfo );

    void FinalizeSolutionStep( ProcessInfo& CurrentProcessInfo );

    //************************************************************************************
    void CalculateOnIntegrationPoints( const Variable<double >& rVariable, std::vector<double>& Output, const ProcessInfo& rCurrentProcessInfo );


    void CalculateOnIntegrationPoints( const Variable<Vector>& rVariable,
                                       std::vector<Vector>& Output, const ProcessInfo& rCurrentProcessInfo );

    void GetValuesVector( Vector& values, int Step );

    void GetValueOnIntegrationPoints( const Variable<Matrix>& rVariable, std::vector<Matrix>& rValues, const ProcessInfo& rCurrentProcessInfo );

    void GetValueOnIntegrationPoints( const Variable<Vector>& rVariable, std::vector<Vector>& rValues, const ProcessInfo& rCurrentProcessInfo );

    void GetValueOnIntegrationPoints( const Variable<double>& rVariable, std::vector<double>& rValues, const ProcessInfo& rCurrentProcessInfo );

    void SetValueOnIntegrationPoints( const Variable<Matrix>& rVariable, std::vector<Matrix>& rValues, const ProcessInfo& rCurrentProcessInfo );

    void SetValueOnIntegrationPoints( const Variable<Vector>& rVariable, std::vector<Vector>& rValues, const ProcessInfo& rCurrentProcessInfo );

    void SetValueOnIntegrationPoints( const Variable<double>& rVariable, std::vector<double>& rValues, const ProcessInfo& rCurrentProcessInfo );

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
//      virtual String Info() const;

    /// Print information about this object.
//      virtual void PrintInfo(std::ostream& rOStream) const;

    /// Print object's data.
//      virtual void PrintData(std::ostream& rOStream) const;


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
    Geometry< Node<3> >::Pointer  mpPressureGeometry;

    std::vector<ConstitutiveLaw::Pointer> mConstitutiveLawVector;

    IntegrationMethod mThisIntegrationMethod;

    double mTotalDomainInitialSize;

    unsigned int mNodesPressMin;
    unsigned int mNodesPressMax;
    unsigned int mNodesDispMin;
    unsigned int mNodesDispMax;

    std::vector< Matrix > mInvJ0;
    Vector mDetJ0;
    ///@}
    ///@name Private Operators
    ///@{
    /** K += weight*Btrans*D*B */
    void CalculateAll( MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector,
                       ProcessInfo& rCurrentProcessInfo,
                       bool CalculateStiffnessMatrixFlag,
                       bool CalculateResidualVectorFlag );

    void CalculateDampingMatrix( MatrixType& rDampingMatrix, ProcessInfo& rCurrentProcessInfo );

    void CalculateBodyForces(
        Vector& BodyForce,
        const ProcessInfo& CurrentProcessInfo
    );

    void InitializeVariables();

    void InitializeMaterial();

    void CalculateAndAddExtForceContribution(
        const Vector& N,
        const ProcessInfo& CurrentProcessInfo,
        Vector& BodyForce,
        VectorType& mResidualVector,
        double weight
    );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //DO TIME INTEGRATION STIFFNESS AND FORCES
    void AssembleTimeSpaceRHSFromSubVectors( VectorType& rRightHandSideVector,
            const Vector& R_U, const Vector& R_W, const Vector& R_A );

    void AssembleTimeSpaceStiffnessFromDampSubMatrices( MatrixType& rLeftHandSideMatrix,
            const Matrix& D_UU, const Matrix& D_UW, const Matrix& D_UA,
            const Matrix&  D_WU, const Matrix&  D_WW, const Matrix&  D_WA,
            const Matrix&  D_AU, const Matrix&  D_AW, const Matrix&  D_AA );

    void AssembleTimeSpaceStiffnessFromStiffSubMatrices( MatrixType& rLeftHandSideMatrix,
            const Matrix& K_UU, const Matrix& K_UW, const Matrix& K_UA,
            const Matrix& K_WU, const Matrix& K_WW, const Matrix& K_WA,
            const Matrix& K_AU, const Matrix& K_AW, const Matrix& K_AA );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //CALCULATE FORCEVECTORS DISPLACEMENT

    void AddBodyForcesToRHSVectorU( Vector& R, Vector& N_DISP, double density, double Weight, double detJ );

    void AddInternalForcesToRHSU
    ( Vector& R, const Matrix& B_Operator,
      Vector& StressVector, double Weight, double detJ );
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //CALCULATE STIFFNESS MATRICES DISPLACEMENT

    void CalculateStiffnesMatrixUU( Matrix& K,
                                    const Matrix& C, const Matrix& B_Operator, const Matrix& DN_DX_DISP,
                                    Vector& N_DISP, double density, double capillaryPressure,
                                    double airPressure, double Weight, double detJ );

    void CalculateStiffnesMatrixUW( Matrix& Help_K_UW, Matrix& tanC_W,
                                    const Matrix& DN_DX_DISP, Vector& N_DISP, Vector& N_PRESS,
                                    double capillaryPressure, double airPressure, double Weight, double DetJ );

    void CalculateStiffnesMatrixUA( Matrix& Help_K_UA, Matrix& tanC_A,
                                    const Matrix& DN_DX_DISP, Vector& N_DISP, Vector& N_PRESS,
                                    double capillaryPressure, double airPressure, double Weight, double DetJ );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //CALCULATE FORCEVECTORS WATER

    void AddInternalForcesToRHSW( Vector& Help_R_U, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double Weight, double  DetJ );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //CALCULATE STIFFNESS MATRICES WATER

    void CalculateStiffnesMatrixWU( Matrix& Help_K_WU, const Matrix& DN_DX, Vector& N, double capillaryPressure, double Weight, double DetJ );

    void CalculateStiffnesMatrixWU
    ( Matrix& Help_K_WU, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );

    void CalculateStiffnesMatrixWW( Matrix& Help_K_WW, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );

    void CalculateStiffnesMatrixWA( Matrix& Help_K_WA, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );


    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //CALCULATE DAMPING MATRICES WATER

    void CalculateDampingMatrixWU
    ( Matrix& Help_D_WU, const Matrix&
      DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );

    void CalculateDampingMatrixWW( Matrix& Help_D_WW, const Matrix& DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );

    void CalculateDampingMatrixWA( Matrix& Help_D_WA, const Matrix& DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //CALCULATE FORCEVECTORS WATER

    void AddInternalForcesToRHSA( Vector& Help_R_A, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double airPressure, double airPressure_Dt, double Weight, double  DetJ );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //CALCULATE STIFFNESS MATRICES AIR

    void CalculateStiffnesMatrixAU( Matrix& Help_K_AU, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double airPressure, double airPressure_Dt, double Weight, double DetJ );

    void CalculateStiffnesMatrixAW( Matrix& Help_K_AW, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double airPressure, double airPressure_Dt, double Weight, double DetJ );

    void CalculateStiffnesMatrixAA( Matrix& Help_K_AA, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double airPressure, double airPressure_Dt, double Weight, double DetJ );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //CALCULATE DAMPING MATRICES AIR

    void CalculateDampingMatrixAU( Matrix& Help_D_AU, const Matrix& DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );

    void CalculateDampingMatrixAW( Matrix& Help_D_AW, const Matrix& DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ );

    void CalculateDampingMatrixAA( Matrix& Help_D_AA, const Matrix& DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double airPressure, double Weight, double DetJ );
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //PRIMARY VARIABLES AND THEIR DERIVATIVES

    void GetDerivativeDPressuresDt( const Vector& N_PRESS, double& capillaryPressure_Dt,
                                    double& waterPressure_Dt, double& airPressure_Dt );

    void GetPressures( const Vector& N, double& capillaryPressure, double& waterPressure, double& airPressure );

    double GetDerivativeDCapillaryPressureDt( const Vector& N_PRESS );

    Vector GetGradWaterPressure( const Matrix& DN_DX_PRESS );

    Vector GetGradAirPressure( const Matrix& DN_DX_PRESS );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //POROSITY AND ITS DERIVATIVES

    double GetPorosity( const Matrix& DN_DX_DISP );

    double GetDerivativeDPorosityDDivU( const Matrix& DN_DX_DISP );
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //DENSITY AIR AND ITS DERIVATIVES

    double GetDensityAir( double airPressure );


    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //AVERAGED DENSITY
    Vector GetGravity();
    double GetDivU( const Matrix& DN_DX_DISP );
    double GetDerivativeDDivUDt( const Matrix& DN_DX_DISP );
    double GetAveragedDensity( double capillaryPressure, double airPressure, double porosity );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //SATURATION AND ITS DERIVATIVES

    double GetSaturation( double capillaryPressure );

    double GetDerivativeDSaturationDpc( double capillaryPressure );

    double GetSecondDerivativeD2SaturationDpc2( double capillaryPressure );

    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************

    //WATER FLOW AND ITS DERIVATIVES

    Vector GetFlowWater( const Matrix& DN_DX, const Matrix& DN_DX_DISP, double capillaryPressure );

    Vector GetDerivativeDWaterFlowDpw( const Matrix& DN_DX, const Matrix& DN_DX_DISP, double capillaryPressure );

    Vector GetDerivativeDWaterFlowDpa( const Matrix& DN_DX, const Matrix& DN_DX_DISP, double capillaryPressure );

    double GetDerivativeDWaterFlowDGradpw( const Matrix& DN_DX_DISP, double capillaryPressure );

    //AIR FLOW AND ITS DERIVATIVES

    Vector GetFlowAir( const Matrix& DN_DX, const Matrix& DN_DX_DISP, double airPressure, double capillaryPressure );

    Vector GetDerivativeDAirFlowDpa( const Matrix& DN_DX, const Matrix& DN_DX_DISP, double airPressure, double capillaryPressure );

    Vector GetDerivativeDAirFlowDpw( const Matrix& DN_DX, const Matrix& DN_DX_DISP, double airPressure, double capillaryPressure );

    double GetDerivativeDAirFlowDGradpa( const Matrix& DN_DX_DISP, double airPressure, double capillaryPressure );
    //************************************************************************************
    //************************************************************************************
    //STRESSES, STRAINS AND CONSTITUTIVE MODELL (UNSATURATED CASE)
    //************************************************************************************
    //************************************************************************************

    void CalculateEffectiveStress( Vector& StressTensor, Matrix& tanC_W, Matrix& tanC_A, const double waterPressure, const double airPressure );

    void  CalculateStressAndTangentialStiffnessUnsaturatedSoils
    ( Vector& StressVector, Matrix& tanC_U,
      Matrix& tanC_W, Matrix& tanC_A, Vector& StrainVector,
      double waterPressure, double airPressure, int PointNumber, const ProcessInfo& rCurrentProcessInfo );
    //************************************************************************************
    //************************************************************************************
    //STRESSES, STRAINS AND CONSTITUTIVE MODELL
    //************************************************************************************
    //************************************************************************************
    void CalculateBoperator( Matrix& B_Operator, const Matrix& DN_DX );

    void CalculateStrain( const Matrix& B, const Matrix& Displacements, Vector& StrainVector );

//    Matrix GetValueOnIntegrationPoints( const Matrix& rVariable, int PointNumber );
//                Matrix CalculateDeformationTensor(const Matrix& DN_DX, unsigned int TimePoints);

    //************************************************************************************
    //************************************************************************************
    //NONLINEAR CONTRIBUTION OF VOLUMECHANGE
    //************************************************************************************
    //************************************************************************************



    ///@}
    ///@name Private Operations
    ///@{


    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Serialization
    ///@{
    friend class Serializer;

    // A private default constructor necessary for serialization
    UnsaturatedSoilsElement_3phase_SmallStrain() {}

    virtual void save(Serializer& rSerializer) const
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer,  Element );
    }

    virtual void load(Serializer& rSerializer)
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer,  Element );
    }

    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    //UnsaturatedSoilsElement_3phase_SmallStrain& operator=(const UnsaturatedSoilsElement_3phase_SmallStrain& rOther);

    /// Copy constructor.
    //UnsaturatedSoilsElement_3phase_SmallStrain(const UnsaturatedSoilsElement_3phase_SmallStrain& rOther);


    ///@}

}; // Class UnsaturatedSoilsElement_3phase_SmallStrain

///@}
///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
/*  inline std::istream& operator >> (std::istream& rIStream,
                   UnsaturatedSoilsElement_3phase_SmallStrain& rThis);
*/
/// output stream function
/*  inline std::ostream& operator << (std::ostream& rOStream,
                   const UnsaturatedSoilsElement_3phase_SmallStrain& rThis)
           {
                   rThis.PrintInfo(rOStream);
                   rOStream << std::endl;
                   rThis.PrintData(rOStream);

                   return rOStream;
}*/
///@}

}  // namespace Kratos.

#endif // KRATOS_UNSATURATED_SOILS_ELEMENT_INCLUDED defined 


