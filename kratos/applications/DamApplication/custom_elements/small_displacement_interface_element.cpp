//   
//   Project Name:        KratosPoromechanicsApplication $
//   Last Modified by:    $Author:              L Gracia $
//   Date:                $Date:              March 2016 $
//   Revision:            $Revision:                 1.0 $
//

// Application includes
#include "custom_elements/small_displacement_interface_element.hpp" 

namespace Kratos
{

template< unsigned int TDim, unsigned int TNumNodes >
Element::Pointer SmallDisplacementInterfaceElement<TDim,TNumNodes>::Create( IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties ) const
{
    return Element::Pointer( new SmallDisplacementInterfaceElement( NewId, GetGeometry().Create( ThisNodes ), pProperties ) );
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::Initialize()
{
    KRATOS_TRY
    
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    const unsigned int NumGPoints = Geom.IntegrationPointsNumber( mThisIntegrationMethod );

    if ( mConstitutiveLawVector.size() != NumGPoints )
        mConstitutiveLawVector.resize( NumGPoints );

    if ( Prop[CONSTITUTIVE_LAW] != NULL )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            mConstitutiveLawVector[i] = Prop[CONSTITUTIVE_LAW]->Clone();
            mConstitutiveLawVector[i]->InitializeMaterial( Prop, Geom,row( Geom.ShapeFunctionsValues( mThisIntegrationMethod ), i ) );
        }
    }
    else
        KRATOS_THROW_ERROR( std::logic_error, "A constitutive law needs to be specified for the element with ID ", this->Id() )
    
    //Compute initial gap of the joint
    this->CalculateInitialGap(Geom);

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
int SmallDisplacementInterfaceElement<TDim,TNumNodes>::Check( const ProcessInfo& rCurrentProcessInfo ) // TODO: ESTO ES NECESARIO???
{
    KRATOS_TRY
    
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();

    //verify that the variables are correctly initialized

    //Solid variables
    if ( DISPLACEMENT.Key() == 0 )
        KRATOS_THROW_ERROR( std::invalid_argument, "DISPLACEMENT has Key zero! (check if the application is correctly registered", "" )

    if ( ACCELERATION.Key() == 0 )
        KRATOS_THROW_ERROR( std::invalid_argument, "ACCELERATION has Key zero! (check if the application is correctly registered", "" )

    if ( DENSITY.Key() == 0 )
        KRATOS_THROW_ERROR( std::invalid_argument, "DENSITY has Key zero! (check if the application is correctly registered", "" )

    //Interface variables
    if ( MINIMUM_JOINT_WIDTH.Key() == 0 )
        KRATOS_THROW_ERROR( std::invalid_argument, "MINIMUM_JOINT_WIDTH has Key zero! (check if the application is correctly registered", "" )

    //verify that the dofs exist
    for ( unsigned int i = 0; i < Geom.size(); i++ )
    {
        if ( Geom[i].SolutionStepsDataHas( DISPLACEMENT ) == false )
            KRATOS_THROW_ERROR( std::invalid_argument, "missing variable DISPLACEMENT on node ", Geom[i].Id() )

        if ( Geom[i].HasDofFor( DISPLACEMENT_X ) == false || Geom[i].HasDofFor( DISPLACEMENT_Y ) == false || Geom[i].HasDofFor( DISPLACEMENT_Z ) == false )
            KRATOS_THROW_ERROR( std::invalid_argument, "missing one of the dofs for the variable DISPLACEMENT on node ", Geom[i].Id() )
    }

    //verify that the constitutive law exists
    if ( Prop.Has( CONSTITUTIVE_LAW ) == false )
        KRATOS_THROW_ERROR( std::logic_error, "constitutive law not provided for property ", Prop.Id() )

	//verify compatibility with the constitutive law
    ConstitutiveLaw::Features LawFeatures;
    Prop.GetValue( CONSTITUTIVE_LAW )->GetLawFeatures(LawFeatures);

    bool correct_strain_measure = false;
    for(unsigned int i=0; i<LawFeatures.mStrainMeasures.size(); i++)
    {
	    if(LawFeatures.mStrainMeasures[i] == ConstitutiveLaw::StrainMeasure_Infinitesimal)
		    correct_strain_measure = true;
    }

    if( correct_strain_measure == false )
	    KRATOS_THROW_ERROR( std::logic_error, "constitutive law is not compatible with the element type ", " StrainMeasure_Infinitesimal " );

    //verify that the constitutive law has the correct Dim
    if ( TDim == 2 )
    {
        if ( Prop.Has( THICKNESS ) == false )
            KRATOS_THROW_ERROR( std::logic_error, "THICKNESS not provided for element ", this->Id() )

        if ( THICKNESS.Key() == 0 )
            KRATOS_THROW_ERROR( std::invalid_argument, "THICKNESS has Key zero! (check if the application is correctly registered)", "" )
    }

	Prop.GetValue( CONSTITUTIVE_LAW )->Check( Prop, Geom, rCurrentProcessInfo );

    return 0;

    KRATOS_CATCH( "" );
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<2,4>::GetDofList( DofsVectorType& rElementalDofList, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY
    
    GeometryType& rGeom = GetGeometry();
    const unsigned int element_size = 4 * 2;
    unsigned int index = 0;
    
    if (rElementalDofList.size() != element_size)
      rElementalDofList.resize( element_size );
    
    for (unsigned int i = 0; i < 4; i++)
    {
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_X);
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_Y);        
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<3,6>::GetDofList( DofsVectorType& rElementalDofList, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY
    
    GeometryType& rGeom = GetGeometry();
    const unsigned int element_size = 6 * 3;
    unsigned int index = 0;
    
    if (rElementalDofList.size() != element_size)
      rElementalDofList.resize( element_size );
    
    for (unsigned int i = 0; i < 6; i++)
    {
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_X);
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_Y);
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_Z);
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<3,8>::GetDofList( DofsVectorType& rElementalDofList, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY
    
    GeometryType& rGeom = GetGeometry();
    const unsigned int element_size = 8 * 3;
    unsigned int index = 0;
    
    if (rElementalDofList.size() != element_size)
      rElementalDofList.resize( element_size );
    
    for (unsigned int i = 0; i < 8; i++)
    {
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_X);
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_Y);
        rElementalDofList[index++] = rGeom[i].pGetDof(DISPLACEMENT_Z);
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateMassMatrix( MatrixType& rMassMatrix, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY
    
    const unsigned int element_size = TNumNodes * TDim ;
    
    //Resizing mass matrix
    if ( rMassMatrix.size1() != element_size )
        rMassMatrix.resize( element_size, element_size, false );
    noalias( rMassMatrix ) = ZeroMatrix( element_size, element_size );

    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    const GeometryType::IntegrationPointsArrayType& integration_points = Geom.IntegrationPoints( mThisIntegrationMethod );
    const unsigned int NumGPoints = integration_points.size();

    //Defining shape functions and the determinant of the jacobian at all integration points
    const Matrix& NContainer = Geom.ShapeFunctionsValues( mThisIntegrationMethod );
    Vector detJContainer(NumGPoints);
    Geom.DeterminantOfJacobian(detJContainer,mThisIntegrationMethod);
    
    //Defining necessary variables
    double IntegrationCoefficient;
    const double Density = Prop[DENSITY];
    boost::numeric::ublas::bounded_matrix<double,TDim, TNumNodes*TDim> Nut = ZeroMatrix(TDim, TNumNodes*TDim);
    array_1d<double,TNumNodes*TDim> DisplacementVector;
    ElementUtilities::GetDisplacementsVector(DisplacementVector,Geom);
    boost::numeric::ublas::bounded_matrix<double,TDim, TDim> RotationMatrix;
    this->CalculateRotationMatrix(RotationMatrix,Geom);
    boost::numeric::ublas::bounded_matrix<double,TDim, TNumNodes*TDim> Nu = ZeroMatrix(TDim, TNumNodes*TDim);
    array_1d<double,TDim> LocalRelDispVector;
    array_1d<double,TDim> RelDispVector;
    const double& MinimumJointWidth = Prop[MINIMUM_JOINT_WIDTH];
    double JointWidth;
    
    //Loop over integration points
    for ( unsigned int GPoint = 0; GPoint < NumGPoints; GPoint++ )
    {
        InterfaceElementUtilities::CalculateNuMatrix(Nu,NContainer,GPoint);

        noalias(RelDispVector) = prod(Nu,DisplacementVector);
            
        noalias(LocalRelDispVector) = prod(RotationMatrix,RelDispVector);
            
        this->CalculateJointWidth(JointWidth, LocalRelDispVector[TDim], MinimumJointWidth,GPoint);

        //InterfaceElementUtilities::CalculateNuElementMatrix(Nut,NContainer,GPoint);
        
        //calculating weighting coefficient for integration
        this->CalculateIntegrationCoefficient( IntegrationCoefficient, detJContainer[GPoint], integration_points[GPoint].Weight() );

        //Adding contribution to Mass matrix
        noalias(rMassMatrix) += Density*prod(trans(Nut),Nut)*JointWidth*IntegrationCoefficient;
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::FinalizeSolutionStep( ProcessInfo& rCurrentProcessInfo )
{   
    KRATOS_TRY
    
    //Defining necessary variables
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    const Matrix& NContainer = Geom.ShapeFunctionsValues( mThisIntegrationMethod );
    array_1d<double,TNumNodes*TDim> DisplacementVector;
    ElementUtilities::GetDisplacementsVector(DisplacementVector,Geom);
    boost::numeric::ublas::bounded_matrix<double,TDim, TDim> RotationMatrix;
    this->CalculateRotationMatrix(RotationMatrix,Geom);
    boost::numeric::ublas::bounded_matrix<double,TDim, TNumNodes*TDim> Nu = ZeroMatrix(TDim, TNumNodes*TDim);
    array_1d<double,TDim> RelDispVector;
    const double& MinimumJointWidth = Prop[MINIMUM_JOINT_WIDTH];
    double JointWidth;

    //Create constitutive law parameters:
    Vector StrainVector(TDim);
    Vector StressVector(TDim);
    Matrix ConstitutiveMatrix(TDim,TDim);
    Vector Np(TNumNodes);
    Matrix GradNpT(TNumNodes,TDim);
    Matrix F = identity_matrix<double>(TDim);
    double detF = 1.0;
    ConstitutiveLaw::Parameters ConstitutiveParameters(Geom,Prop,rCurrentProcessInfo);
    ConstitutiveParameters.SetConstitutiveMatrix(ConstitutiveMatrix);
    ConstitutiveParameters.SetStressVector(StressVector);
    ConstitutiveParameters.SetStrainVector(StrainVector);
    ConstitutiveParameters.SetShapeFunctionsValues(Np);
    ConstitutiveParameters.SetShapeFunctionsDerivatives(GradNpT);
    ConstitutiveParameters.SetDeterminantF(detF);
    ConstitutiveParameters.SetDeformationGradientF(F);
        
    //Loop over integration points
    for ( unsigned int GPoint = 0; GPoint < mConstitutiveLawVector.size(); GPoint++ )
    {
        InterfaceElementUtilities::CalculateNuMatrix(Nu,NContainer,GPoint);

        noalias(RelDispVector) = prod(Nu,DisplacementVector);
    
        noalias(StrainVector) = prod(RotationMatrix,RelDispVector);
        
        this->CheckAndCalculateJointWidth(JointWidth, ConstitutiveParameters, StrainVector[TDim], MinimumJointWidth, GPoint); //TODO PORQUE ES TDim -1
        
        noalias(Np) = row(NContainer,GPoint);
        
        //compute constitutive tensor and/or stresses
        mConstitutiveLawVector[GPoint]->FinalizeMaterialResponseCauchy(ConstitutiveParameters);
    }
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateLocalSystem( MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    unsigned int element_size = TNumNodes * TDim;
    
    //Resetting the LHS
    if ( rLeftHandSideMatrix.size1() != element_size )
        rLeftHandSideMatrix.resize( element_size, element_size, false );
    noalias( rLeftHandSideMatrix ) = ZeroMatrix( element_size, element_size );
    
    //Resetting the RHS
    if ( rRightHandSideVector.size() != element_size )
        rRightHandSideVector.resize( element_size, false );
    noalias( rRightHandSideVector ) = ZeroVector( element_size );
    
    this->CalculateAll(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
        
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateRightHandSide( VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY
    
    unsigned int element_size = TNumNodes * TDim;
        
    //Resetting the RHS
    if ( rRightHandSideVector.size() != element_size )
        rRightHandSideVector.resize( element_size, false );
    noalias( rRightHandSideVector ) = ZeroVector( element_size );
    
    this->CalculateRHS(rRightHandSideVector, rCurrentProcessInfo);
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<2,4>::EquationIdVector( EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    GeometryType& rGeom = GetGeometry();
    const unsigned int element_size = 4 * 2;
    unsigned int index = 0;
    
    if (rResult.size() != element_size)
      rResult.resize( element_size, false );

    for (unsigned int i = 0; i < 4; i++)
    {
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_X).EquationId();
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_Y).EquationId();
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template<  >
void SmallDisplacementInterfaceElement<3,6>::EquationIdVector( EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    GeometryType& rGeom = GetGeometry();
    const unsigned int element_size = 6 * 3;
    unsigned int index = 0;
    
    if (rResult.size() != element_size)
      rResult.resize( element_size, false );

    for (unsigned int i = 0; i < 6; i++)
    {
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_X).EquationId();
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_Y).EquationId();
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_Z).EquationId();
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template<  >
void SmallDisplacementInterfaceElement<3,8>::EquationIdVector( EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    GeometryType& rGeom = GetGeometry();
    const unsigned int element_size = 8 * 3;
    unsigned int index = 0;
    
    if (rResult.size() != element_size)
      rResult.resize( element_size, false );

    for (unsigned int i = 0; i < 8; i++)
    {
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_X).EquationId();
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_Y).EquationId();
        rResult[index++] = rGeom[i].GetDof(DISPLACEMENT_Z).EquationId();
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::GetSecondDerivativesVector( Vector& rValues, int Step )
{
    const GeometryType& Geom = GetGeometry();
    const unsigned int element_size = TNumNodes * TDim;
    unsigned int index = 0;

    if ( rValues.size() != element_size )
        rValues.resize( element_size, false );

    for ( unsigned int i = 0; i < TNumNodes; i++ )
    {
        rValues[index++] = Geom[i].FastGetSolutionStepValue( ACCELERATION_X, Step );
        rValues[index++] = Geom[i].FastGetSolutionStepValue( ACCELERATION_Y, Step );
        if ( TDim > 2 )
            rValues[index++] = Geom[i].FastGetSolutionStepValue( ACCELERATION_Z, Step );
        rValues[index++] = 0.0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::SetValueOnIntegrationPoints( const Variable<double>& rVariable,
                                                        std::vector<double>& rValues,const ProcessInfo& rCurrentProcessInfo )
{
    for ( unsigned int GPoint = 0; GPoint < mConstitutiveLawVector.size(); GPoint++ )
        mConstitutiveLawVector[GPoint]->SetValue( rVariable, rValues[GPoint], rCurrentProcessInfo );
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::GetValueOnIntegrationPoints( const Variable<double>& rVariable,
                                                                                    std::vector<double>& rValues,const ProcessInfo& rCurrentProcessInfo )
{
    if(rVariable == DAMAGE_VARIABLE)
    {
        //Variables computed on Lobatto points
        const GeometryType& Geom = this->GetGeometry();
        const unsigned int NumGPoints = Geom.IntegrationPointsNumber( mThisIntegrationMethod );
        std::vector<double> GPValues(NumGPoints);
        
        for ( unsigned int i = 0;  i < NumGPoints; i++ )
            GPValues[i] = mConstitutiveLawVector[i]->GetValue( rVariable, GPValues[i] );
        
        //Printed on standard GiD Gauss points
        const unsigned int OutputGPoints = Geom.IntegrationPointsNumber( GeometryData::GI_GAUSS_2 );    
        if ( rValues.size() != OutputGPoints )
            rValues.resize( OutputGPoints );
        
        this->CalculateOutputDoubles(rValues,GPValues);
    }
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::GetValueOnIntegrationPoints(const Variable<array_1d<double,3>>& rVariable,
                                                                                    std::vector<array_1d<double,3>>& rValues,const ProcessInfo& rCurrentProcessInfo)
{
    if(rVariable == LOCAL_STRESS_VECTOR || rVariable == LOCAL_RELATIVE_DISPLACEMENT_VECTOR ) // TODO: FLAGS ESPECIALES???
    {
        //Variables computed on Lobatto points
        const GeometryType& Geom = this->GetGeometry();
        std::vector<array_1d<double,3>> GPValues(Geom.IntegrationPointsNumber( mThisIntegrationMethod ));
            
        this->CalculateOnIntegrationPoints(rVariable, GPValues, rCurrentProcessInfo);
        
        //Printed on standard GiD Gauss points
        const unsigned int OutputGPoints = Geom.IntegrationPointsNumber( GeometryData::GI_GAUSS_2 );    
        if ( rValues.size() != OutputGPoints )
            rValues.resize( OutputGPoints );

        this->CalculateOutputValues< array_1d<double,3> >(rValues,GPValues);
    }
}


//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateOnIntegrationPoints( const Variable<array_1d<double,3>>& rVariable, 
                                                                                std::vector<array_1d<double,3>>& rOutput, const ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY
    
    if(rVariable == LOCAL_STRESS_VECTOR)
    {
        //Defining necessary variables
        const PropertiesType& Prop = this->GetProperties();
        const GeometryType& Geom = this->GetGeometry();
        const Matrix& NContainer = Geom.ShapeFunctionsValues( mThisIntegrationMethod );
        array_1d<double,TNumNodes*TDim> DisplacementVector;
        ElementUtilities::GetDisplacementsVector(DisplacementVector,Geom);
        boost::numeric::ublas::bounded_matrix<double,TDim, TDim> RotationMatrix;
        this->CalculateRotationMatrix(RotationMatrix,Geom);
        boost::numeric::ublas::bounded_matrix<double,TDim, TNumNodes*TDim> Nu = ZeroMatrix(TDim, TNumNodes*TDim);
        array_1d<double,TDim> RelDispVector;
        const double& MinimumJointWidth = Prop[MINIMUM_JOINT_WIDTH];
        double JointWidth;
        array_1d<double,TDim> LocalStressVector;
        
        //Create constitutive law parameters:
        Vector StrainVector(TDim);
        Vector StressVectorDynamic(TDim);
        Matrix ConstitutiveMatrix(TDim,TDim);
        Vector Np(TNumNodes);
        Matrix GradNpT(TNumNodes,TDim);
        Matrix F = identity_matrix<double>(TDim);
        double detF = 1.0;
        ConstitutiveLaw::Parameters ConstitutiveParameters(Geom,Prop,rCurrentProcessInfo);
        ConstitutiveParameters.Set(ConstitutiveLaw::COMPUTE_STRESS);
        ConstitutiveParameters.SetConstitutiveMatrix(ConstitutiveMatrix);
        ConstitutiveParameters.SetStressVector(StressVectorDynamic);
        ConstitutiveParameters.SetStrainVector(StrainVector);
        ConstitutiveParameters.SetShapeFunctionsValues(Np);
        ConstitutiveParameters.SetShapeFunctionsDerivatives(GradNpT);
        ConstitutiveParameters.SetDeterminantF(detF);
        ConstitutiveParameters.SetDeformationGradientF(F);
        
        //Loop over integration points
        for ( unsigned int GPoint = 0; GPoint < mConstitutiveLawVector.size(); GPoint++ )
        {
            InterfaceElementUtilities::CalculateNuMatrix(Nu,NContainer,GPoint);

            noalias(RelDispVector) = prod(Nu,DisplacementVector);
            
            noalias(StrainVector) = prod(RotationMatrix,RelDispVector);
            
            this->CheckAndCalculateJointWidth(JointWidth, ConstitutiveParameters, StrainVector[TDim], MinimumJointWidth, GPoint);
            
            noalias(Np) = row(NContainer,GPoint);
            
            //compute constitutive tensor and/or stresses
            mConstitutiveLawVector[GPoint]->CalculateMaterialResponseCauchy(ConstitutiveParameters);
            
            noalias(LocalStressVector) = StressVectorDynamic;
            
            ElementUtilities::FillArray1dOutput(rOutput[GPoint],LocalStressVector);
        }
    }
    else if(rVariable == LOCAL_RELATIVE_DISPLACEMENT_VECTOR)
    {
        //Defining necessary variables
        const GeometryType& Geom = this->GetGeometry();
        const Matrix& NContainer = Geom.ShapeFunctionsValues( mThisIntegrationMethod );
        array_1d<double,TNumNodes*TDim> DisplacementVector;
        ElementUtilities::GetDisplacementsVector(DisplacementVector,Geom);
        boost::numeric::ublas::bounded_matrix<double,TDim, TDim> RotationMatrix;
        this->CalculateRotationMatrix(RotationMatrix,Geom);
        boost::numeric::ublas::bounded_matrix<double,TDim, TNumNodes*TDim> Nu = ZeroMatrix(TDim, TNumNodes*TDim);
        array_1d<double,TDim> LocalRelDispVector;
        array_1d<double,TDim> RelDispVector;
                
        //Loop over integration points
        for ( unsigned int GPoint = 0; GPoint < mConstitutiveLawVector.size(); GPoint++ )
        {
            InterfaceElementUtilities::CalculateNuMatrix(Nu,NContainer,GPoint);

            noalias(RelDispVector) = prod(Nu,DisplacementVector);
            
            noalias(LocalRelDispVector) = prod(RotationMatrix,RelDispVector);
                        
            ElementUtilities::FillArray1dOutput(rOutput[GPoint],LocalRelDispVector);
        }
    }
        
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<2,4>::CalculateInitialGap(const GeometryType& Geom)
{
    mInitialGap.resize(2,false);
    
    array_1d<double,3> Vx;
    noalias(Vx) = Geom.GetPoint( 3 ) - Geom.GetPoint( 0 );
    mInitialGap[0] = norm_2(Vx);

    noalias(Vx) = Geom.GetPoint( 2 ) - Geom.GetPoint( 1 );
    mInitialGap[1] = norm_2(Vx);
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<3,6>::CalculateInitialGap(const GeometryType& Geom)
{
    mInitialGap.resize(3,false);
    
    array_1d<double,3> Vx;
    noalias(Vx) = Geom.GetPoint( 3 ) - Geom.GetPoint( 0 );
    mInitialGap[0] = norm_2(Vx);

    noalias(Vx) = Geom.GetPoint( 4 ) - Geom.GetPoint( 1 );
    mInitialGap[1] = norm_2(Vx);

    noalias(Vx) = Geom.GetPoint( 5 ) - Geom.GetPoint( 2 );
    mInitialGap[2] = norm_2(Vx);
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<3,8>::CalculateInitialGap(const GeometryType& Geom)
{
    mInitialGap.resize(4,false);
    
    array_1d<double,3> Vx;
    noalias(Vx) = Geom.GetPoint( 4 ) - Geom.GetPoint( 0 );
    mInitialGap[0] = norm_2(Vx);

    noalias(Vx) = Geom.GetPoint( 5 ) - Geom.GetPoint( 1 );
    mInitialGap[1] = norm_2(Vx);

    noalias(Vx) = Geom.GetPoint( 6 ) - Geom.GetPoint( 2 );
    mInitialGap[2] = norm_2(Vx);

    noalias(Vx) = Geom.GetPoint( 7 ) - Geom.GetPoint( 3 );
    mInitialGap[3] = norm_2(Vx);
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateStiffnessMatrix( MatrixType& rStiffnessMatrix, const ProcessInfo& CurrentProcessInfo )
{    
    KRATOS_TRY

    const unsigned int element_size = TNumNodes * TDim;
    
    //Resizing mass matrix
    if ( rStiffnessMatrix.size1() != element_size )
        rStiffnessMatrix.resize( element_size, element_size, false );
    noalias( rStiffnessMatrix ) = ZeroMatrix( element_size, element_size );
    
    //Previous definitions 
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    const GeometryType::IntegrationPointsArrayType& integration_points = Geom.IntegrationPoints( mThisIntegrationMethod );
    const unsigned int NumGPoints = integration_points.size();
    
    //Containers of variables at all integration points
    const Matrix& NContainer = Geom.ShapeFunctionsValues( mThisIntegrationMethod );
    GeometryType::JacobiansType JContainer(NumGPoints);
    Geom.Jacobian( JContainer, mThisIntegrationMethod );
    Vector detJContainer(NumGPoints);
    Geom.DeterminantOfJacobian(detJContainer,mThisIntegrationMethod);

    //Constitutive Law parameters
    ConstitutiveLaw::Parameters ConstitutiveParameters(Geom,Prop,CurrentProcessInfo);
    ConstitutiveParameters.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR);
    
    //Element variables
    ElementVariables Variables;
    this->InitializeElementVariables(Variables,ConstitutiveParameters,Geom,Prop,CurrentProcessInfo);
    
    //Auxiliary variables
    const double& MinimumJointWidth = Prop[MINIMUM_JOINT_WIDTH];
    array_1d<double,TDim> RelDispVector;
    
    
    //Loop over integration points
    for( unsigned int GPoint = 0; GPoint < NumGPoints; GPoint++)
    {
        //Compute Np, StrainVector, JointWidth, GradNpT
        noalias(Variables.Np) = row(NContainer,GPoint);
        InterfaceElementUtilities::CalculateNuMatrix(Variables.Nu,NContainer,GPoint);
        noalias(RelDispVector) = prod(Variables.Nu,Variables.DisplacementVector);
        noalias(Variables.StrainVector) = prod(Variables.RotationMatrix,RelDispVector);
        this->CheckAndCalculateJointWidth(Variables.JointWidth,ConstitutiveParameters,Variables.StrainVector[TDim], MinimumJointWidth, GPoint);
        
        
        //Compute constitutive tensor
        mConstitutiveLawVector[GPoint]->CalculateMaterialResponseCauchy(ConstitutiveParameters);

        //Compute weighting coefficient for integration
        this->CalculateIntegrationCoefficient(Variables.IntegrationCoefficient, detJContainer[GPoint], integration_points[GPoint].Weight() );
        
        //Compute stiffness matrix
        this->CalculateAndAddStiffnessMatrix(rStiffnessMatrix, Variables);
    }
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------


template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateAll( MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, const ProcessInfo& rCurrentProcessInfo )
{    
    KRATOS_TRY
    
    //Previous definitions 
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    const GeometryType::IntegrationPointsArrayType& integration_points = Geom.IntegrationPoints( mThisIntegrationMethod );
    const unsigned int NumGPoints = integration_points.size();
    
    //Containers of variables at all integration points
    const Matrix& NContainer = Geom.ShapeFunctionsValues( mThisIntegrationMethod );
    GeometryType::JacobiansType JContainer(NumGPoints);
    Geom.Jacobian( JContainer, mThisIntegrationMethod );
    Vector detJContainer(NumGPoints);
    Geom.DeterminantOfJacobian(detJContainer,mThisIntegrationMethod);

    //Constitutive Law parameters
    ConstitutiveLaw::Parameters ConstitutiveParameters(Geom,Prop,rCurrentProcessInfo);
    ConstitutiveParameters.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR);
    ConstitutiveParameters.Set(ConstitutiveLaw::COMPUTE_STRESS);
    
    //Element variables
    ElementVariables Variables; 
    this->InitializeElementVariables(Variables,ConstitutiveParameters,Geom,Prop,rCurrentProcessInfo);
    
    //Auxiliary variables
    const double& MinimumJointWidth = Prop[MINIMUM_JOINT_WIDTH];
    array_1d<double,TDim> RelDispVector;

    
    //Loop over integration points
    for( unsigned int GPoint = 0; GPoint < NumGPoints; GPoint++)
    {
        //Compute Np, StrainVector, JointWidth, GradNpT
        noalias(Variables.Np) = row(NContainer,GPoint);
        InterfaceElementUtilities::CalculateNuMatrix(Variables.Nu,NContainer,GPoint);
        noalias(RelDispVector) = prod(Variables.Nu,Variables.DisplacementVector);
        noalias(Variables.StrainVector) = prod(Variables.RotationMatrix,RelDispVector);
        this->CheckAndCalculateJointWidth(Variables.JointWidth,ConstitutiveParameters,Variables.StrainVector[TDim], MinimumJointWidth, GPoint);
        
        //Compute BodyAcceleration
        ElementUtilities::InterpolateVariableWithComponents(Variables.BodyAcceleration,NContainer,Variables.VolumeAcceleration,GPoint);
               
        //Compute constitutive tensor and stresses
        mConstitutiveLawVector[GPoint]->CalculateMaterialResponseCauchy(ConstitutiveParameters);

        //Compute weighting coefficient for integration
        this->CalculateIntegrationCoefficient(Variables.IntegrationCoefficient, detJContainer[GPoint], integration_points[GPoint].Weight() );
        
        //Contributions to the left hand side
        this->CalculateAndAddLHS(rLeftHandSideMatrix, Variables);
        
        //Contributions to the right hand side
        this->CalculateAndAddRHS(rRightHandSideVector, Variables);
    }
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateRHS( VectorType& rRightHandSideVector, const ProcessInfo& rCurrentProcessInfo )
{    
    KRATOS_TRY
       
    //Previous definitions 
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    const GeometryType::IntegrationPointsArrayType& integration_points = Geom.IntegrationPoints( mThisIntegrationMethod );
    const unsigned int NumGPoints = integration_points.size();
    
    //Containers of variables at all integration points
    const Matrix& NContainer = Geom.ShapeFunctionsValues( mThisIntegrationMethod );
    GeometryType::JacobiansType JContainer(NumGPoints);
    Geom.Jacobian( JContainer, mThisIntegrationMethod );
    Vector detJContainer(NumGPoints);
    Geom.DeterminantOfJacobian(detJContainer,mThisIntegrationMethod);

    //Constitutive Law parameters
    ConstitutiveLaw::Parameters ConstitutiveParameters(Geom,Prop,rCurrentProcessInfo);
    ConstitutiveParameters.Set(ConstitutiveLaw::COMPUTE_STRESS);
    
    //Element variables
    ElementVariables Variables;
    this->InitializeElementVariables(Variables,ConstitutiveParameters,Geom,Prop,rCurrentProcessInfo);
    
    //Auxiliary variables
    const double& MinimumJointWidth = Prop[MINIMUM_JOINT_WIDTH];
    array_1d<double,TDim> RelDispVector;
    
    //Loop over integration points
    for( unsigned int GPoint = 0; GPoint < NumGPoints; GPoint++)
    {
        //Compute Np, StrainVector, JointWidth, GradNpT
        noalias(Variables.Np) = row(NContainer,GPoint);
        InterfaceElementUtilities::CalculateNuMatrix(Variables.Nu,NContainer,GPoint);
        noalias(RelDispVector) = prod(Variables.Nu,Variables.DisplacementVector);
        noalias(Variables.StrainVector) = prod(Variables.RotationMatrix,RelDispVector);        
        this->CheckAndCalculateJointWidth(Variables.JointWidth,ConstitutiveParameters,Variables.StrainVector[TDim], MinimumJointWidth, GPoint);

        
        //Compute BodyAcceleration
        ElementUtilities::InterpolateVariableWithComponents(Variables.BodyAcceleration,NContainer,Variables.VolumeAcceleration,GPoint);

        //Compute constitutive tensor and stresses
        mConstitutiveLawVector[GPoint]->CalculateMaterialResponseCauchy(ConstitutiveParameters);

        //Compute weighting coefficient for integration
        this->CalculateIntegrationCoefficient(Variables.IntegrationCoefficient, detJContainer[GPoint], integration_points[GPoint].Weight() );
        
        //Contributions to the right hand side
        this->CalculateAndAddRHS(rRightHandSideVector, Variables);
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::InitializeElementVariables(ElementVariables& rVariables,ConstitutiveLaw::Parameters& rConstitutiveParameters,
                                                                                  const GeometryType& Geom, const PropertiesType& Prop, const ProcessInfo& CurrentProcessInfo)
{   
    KRATOS_TRY
    
    //Properties variables    
    rVariables.Density = Prop[DENSITY];

    ElementUtilities::GetDisplacementsVector(rVariables.DisplacementVector,Geom);
    ElementUtilities::GetVolumeAccelerationVector(rVariables.VolumeAcceleration,Geom);
    
    //General Variables
    this->CalculateRotationMatrix(rVariables.RotationMatrix,Geom);
    InterfaceElementUtilities::CalculateVoigtVector(rVariables.VoigtVector);
    
    //Variables computed at each GP
    //Constitutive Law parameters
    rVariables.StrainVector.resize(TDim,false);
    rVariables.StressVector.resize(TDim,false);
    rVariables.ConstitutiveMatrix.resize(TDim,TDim,false);
    rVariables.Np.resize(TNumNodes,false);
    rVariables.GradNpT.resize(TNumNodes,TDim,false);
    rVariables.F.resize(TDim,TDim,false);
    rVariables.detF = 1.0;
    rConstitutiveParameters.SetStrainVector(rVariables.StrainVector);
    rConstitutiveParameters.SetStressVector(rVariables.StressVector);
    rConstitutiveParameters.SetConstitutiveMatrix(rVariables.ConstitutiveMatrix);
    rConstitutiveParameters.SetShapeFunctionsValues(rVariables.Np);
    rConstitutiveParameters.SetShapeFunctionsDerivatives(rVariables.GradNpT);
    rConstitutiveParameters.SetDeformationGradientF(rVariables.F);
    rConstitutiveParameters.SetDeterminantF(rVariables.detF);
    //Auxiliary variables
    noalias(rVariables.Nu) = ZeroMatrix(TDim, TNumNodes*TDim);
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template<>
void SmallDisplacementInterfaceElement<2,4>::CalculateRotationMatrix(boost::numeric::ublas::bounded_matrix<double,2,2>& rRotationMatrix, const GeometryType& Geom)
{
    KRATOS_TRY
    
    //Define mid-plane points for quadrilateral_interface_2d_4    
    array_1d<double, 3> pmid0;
    array_1d<double, 3> pmid1;
    noalias(pmid0) = 0.5 * (Geom.GetPoint( 0 ) + Geom.GetPoint( 3 ));
    noalias(pmid1) = 0.5 * (Geom.GetPoint( 1 ) + Geom.GetPoint( 2 ));
    
    //Unitary vector in local x direction
    array_1d<double, 3> Vx;
    noalias(Vx) = pmid1 - pmid0;
    double inv_norm_x = 1.0/norm_2(Vx);
    Vx[0] *= inv_norm_x;
    Vx[1] *= inv_norm_x;
        
    //Rotation Matrix
    rRotationMatrix(0,0) = Vx[0];
    rRotationMatrix(0,1) = Vx[1];
    
    rRotationMatrix(1,0) = -Vx[1];
    rRotationMatrix(1,1) = Vx[0];
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template<>
void SmallDisplacementInterfaceElement<3,6>::CalculateRotationMatrix(boost::numeric::ublas::bounded_matrix<double,3,3>& rRotationMatrix, const GeometryType& Geom)
{
    KRATOS_TRY
    
    //Define mid-plane points for prism_interface_3d_6
    array_1d<double, 3> pmid0;
    array_1d<double, 3> pmid1;
    array_1d<double, 3> pmid2;
    noalias(pmid0) = 0.5 * (Geom.GetPoint( 0 ) + Geom.GetPoint( 3 ));
    noalias(pmid1) = 0.5 * (Geom.GetPoint( 1 ) + Geom.GetPoint( 4 ));
    noalias(pmid2) = 0.5 * (Geom.GetPoint( 2 ) + Geom.GetPoint( 5 ));
    
    //Unitary vector in local x direction
    array_1d<double, 3> Vx;
    noalias(Vx) = pmid1 - pmid0;
    double inv_norm_x = 1.0/norm_2(Vx);
    Vx[0] *= inv_norm_x;
    Vx[1] *= inv_norm_x;
    Vx[2] *= inv_norm_x;
        
    //Unitary vector in local z direction
    array_1d<double, 3> Vy;
    noalias(Vy) = pmid2 - pmid0;
    array_1d<double, 3> Vz;
    MathUtils<double>::CrossProduct(Vz, Vx, Vy);
    double inv_norm_z = 1.0/norm_2(Vz);
    Vz[0] *= inv_norm_z;
    Vz[1] *= inv_norm_z;
    Vz[2] *= inv_norm_z;
            
    //Unitary vector in local y direction
    MathUtils<double>::CrossProduct( Vy, Vz, Vx);
    
    //Rotation Matrix
    rRotationMatrix(0,0) = Vx[0];
    rRotationMatrix(0,1) = Vx[1];
    rRotationMatrix(0,2) = Vx[2];
    
    rRotationMatrix(1,0) = Vy[0];
    rRotationMatrix(1,1) = Vy[1];
    rRotationMatrix(1,2) = Vy[2];
    
    rRotationMatrix(2,0) = Vz[0];
    rRotationMatrix(2,1) = Vz[1];
    rRotationMatrix(2,2) = Vz[2];
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template<>
void SmallDisplacementInterfaceElement<3,8>::CalculateRotationMatrix(boost::numeric::ublas::bounded_matrix<double,3,3>& rRotationMatrix, const GeometryType& Geom)
{
    KRATOS_TRY
    
    //Define mid-plane points for hexahedra_interface_3d_8
    array_1d<double, 3> pmid0;
    array_1d<double, 3> pmid1;
    array_1d<double, 3> pmid2;
    noalias(pmid0) = 0.5 * (Geom.GetPoint( 0 ) + Geom.GetPoint( 4 ));
    noalias(pmid1) = 0.5 * (Geom.GetPoint( 1 ) + Geom.GetPoint( 5 ));
    noalias(pmid2) = 0.5 * (Geom.GetPoint( 2 ) + Geom.GetPoint( 6 ));
    
    //Unitary vector in local x direction
    array_1d<double, 3> Vx;
    noalias(Vx) = pmid1 - pmid0;
    double inv_norm_x = 1.0/norm_2(Vx);
    Vx[0] *= inv_norm_x;
    Vx[1] *= inv_norm_x;
    Vx[2] *= inv_norm_x;
    
    //Unitary vector in local z direction
    array_1d<double, 3> Vy;
    noalias(Vy) = pmid2 - pmid0;
    array_1d<double, 3> Vz;
    MathUtils<double>::CrossProduct(Vz, Vx, Vy);
    double inv_norm_z = 1.0/norm_2(Vz);
    Vz[0] *= inv_norm_z;
    Vz[1] *= inv_norm_z;
    Vz[2] *= inv_norm_z;
    
    //Unitary vector in local y direction
    MathUtils<double>::CrossProduct( Vy, Vz, Vx);
    
    //Rotation Matrix
    rRotationMatrix(0,0) = Vx[0];
    rRotationMatrix(0,1) = Vx[1];
    rRotationMatrix(0,2) = Vx[2];
    
    rRotationMatrix(1,0) = Vy[0];
    rRotationMatrix(1,1) = Vy[1];
    rRotationMatrix(1,2) = Vy[2];
    
    rRotationMatrix(2,0) = Vz[0];
    rRotationMatrix(2,1) = Vz[1];
    rRotationMatrix(2,2) = Vz[2];
    
    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateJointWidth(double& rJointWidth, const double& NormalRelDisp,
                                                                        const double& MinimumJointWidth,const unsigned int& GPoint)
{
    rJointWidth = mInitialGap[GPoint] + NormalRelDisp;
    
    if(rJointWidth < MinimumJointWidth)
    {
        rJointWidth = MinimumJointWidth;
    }
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CheckAndCalculateJointWidth(double& rJointWidth, ConstitutiveLaw::Parameters& rConstitutiveParameters, 
                                                                                double& rNormalRelDisp,const double& MinimumJointWidth,const unsigned int& GPoint)
{
    rJointWidth = mInitialGap[GPoint] + rNormalRelDisp;
    
    rConstitutiveParameters.Set(ConstitutiveLaw::COMPUTE_STRAIN_ENERGY); // No contact between interfaces //TODO: TEMA FLAGS
    
    if(rJointWidth < 0.0)
    {
        rConstitutiveParameters.Reset(ConstitutiveLaw::COMPUTE_STRAIN_ENERGY); // Contact between interfaces //TODO: TEMA FLAGS
        rNormalRelDisp = rJointWidth;
        rJointWidth = MinimumJointWidth;
    }
    else if(rJointWidth < MinimumJointWidth)
    {
        rJointWidth = MinimumJointWidth;
    }
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<2,4>::CalculateIntegrationCoefficient(double& rIntegrationCoefficient, const double& weight, const double& detJ)
{
    rIntegrationCoefficient = weight * detJ * GetProperties()[THICKNESS];
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<3,6>::CalculateIntegrationCoefficient(double& rIntegrationCoefficient, const double& weight, const double& detJ)
{
    rIntegrationCoefficient = weight * detJ;
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<3,8>::CalculateIntegrationCoefficient(double& rIntegrationCoefficient, const double& weight, const double& detJ)
{
    rIntegrationCoefficient = weight * detJ;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateAndAddLHS(MatrixType& rLeftHandSideMatrix, ElementVariables& rVariables)
{
    this->CalculateAndAddStiffnessMatrix(rLeftHandSideMatrix,rVariables);
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateAndAddStiffnessMatrix(MatrixType& rLeftHandSideMatrix, ElementVariables& rVariables)
{
    noalias(rVariables.DimMatrix) = prod(trans(rVariables.RotationMatrix),
                                        boost::numeric::ublas::bounded_matrix<double,TDim,TDim>(prod(rVariables.ConstitutiveMatrix,
                                        rVariables.RotationMatrix)));
    noalias(rVariables.UDimMatrix) = prod(trans(rVariables.Nu),rVariables.DimMatrix); // TODO: ES NECESARIA??? ES usada tambien CalculateAndAddStifnessForce
    noalias(rVariables.UMatrix) = prod(rVariables.UDimMatrix,rVariables.Nu)*rVariables.IntegrationCoefficient;
    
    noalias(rLeftHandSideMatrix) += rVariables.UMatrix;
    
    //Distribute stiffness block matrix into the elemental matrix
    //ElementUtilities::AssembleUBlockMatrix(rLeftHandSideMatrix,rVariables.UMatrix);
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateAndAddRHS(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{
    this->CalculateAndAddStiffnessForce(rRightHandSideVector, rVariables);

    this->CalculateAndAddMixBodyForce(rRightHandSideVector, rVariables);
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateAndAddStiffnessForce(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{
    noalias(rVariables.UDimMatrix) = prod(trans(rVariables.Nu),trans(rVariables.RotationMatrix));
    
    noalias(rVariables.UVector) = -1.0*prod(rVariables.UDimMatrix,rVariables.StressVector)*rVariables.IntegrationCoefficient;
    
    noalias(rRightHandSideVector) += rVariables.UVector;
    //Distribute stiffness block vector into elemental vector
    //ElementUtilities::AssembleUBlockVector(rRightHandSideVector,rVariables.UVector);
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SmallDisplacementInterfaceElement<TDim,TNumNodes>::CalculateAndAddMixBodyForce(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{
    noalias(rVariables.UVector) = rVariables.Density*prod(trans(rVariables.Nu),rVariables.BodyAcceleration)*rVariables.JointWidth*rVariables.IntegrationCoefficient;
    
    noalias(rRightHandSideVector) += rVariables.UVector;
    
    //Distribute body force block vector into elemental vector
    //ElementUtilities::AssembleUBlockVector(rRightHandSideVector,rVariables.UVector);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<2,4>::CalculateOutputDoubles( std::vector<double>& rOutput, const std::vector<double>& GPValues )
{
    //Interpolation of computed values at Lobatto GP to the standard GiD gauss points
    
    rOutput[0] = 0.6220084679281462 * GPValues[0] + 0.16666666666666663 * GPValues[1] + 0.044658198738520435 * GPValues[1] + 0.16666666666666663 * GPValues[0];
    
    rOutput[1] = 0.16666666666666663 * GPValues[0] + 0.6220084679281462 * GPValues[1] + 0.16666666666666663 * GPValues[1] + 0.044658198738520435 * GPValues[0];
    
    rOutput[2]= 0.044658198738520435 * GPValues[0] + 0.16666666666666663 * GPValues[1] + 0.6220084679281462 * GPValues[1] + 0.16666666666666663 * GPValues[0];
    
    rOutput[3] = 0.16666666666666663 * GPValues[0] + 0.044658198738520435 * GPValues[1] + 0.16666666666666663 * GPValues[1] + 0.6220084679281462 * GPValues[0];
}

//----------------------------------------------------------------------------------------

template< >
void SmallDisplacementInterfaceElement<3,6>::CalculateOutputDoubles( std::vector<double>& rOutput, const std::vector<double>& GPValues )
{
    //Interpolation of computed values at Lobatto GP to the standard GiD gauss points
    
    rOutput[0] = 0.5257834230632086 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.13144585576580214 * GPValues[2]
                + 0.14088324360345805 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.03522081090086451 * GPValues[2];
               
    rOutput[1] = 0.13144585576580214 * GPValues[0] + 0.5257834230632086 * GPValues[1] + 0.13144585576580214 * GPValues[2]
                + 0.03522081090086451 * GPValues[0] + 0.14088324360345805 * GPValues[1] + 0.03522081090086451 * GPValues[2];
               
    rOutput[2] = 0.13144585576580214 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.5257834230632086 * GPValues[2]
                + 0.03522081090086451 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.14088324360345805 * GPValues[2];
               
    rOutput[3] = 0.14088324360345805 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.03522081090086451 * GPValues[2]
                + 0.5257834230632086 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.13144585576580214 * GPValues[2];
               
    rOutput[4] = 0.03522081090086451 * GPValues[0] + 0.14088324360345805 * GPValues[1] + 0.03522081090086451 * GPValues[2]
                + 0.13144585576580214 * GPValues[0] + 0.5257834230632086 * GPValues[1] + 0.13144585576580214 * GPValues[2];
               
    rOutput[5] = 0.03522081090086451 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.14088324360345805 * GPValues[2]
                + 0.13144585576580214 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.5257834230632086 * GPValues[2];
}

//----------------------------------------------------------------------------------------

template<>
void SmallDisplacementInterfaceElement<3,8>::CalculateOutputDoubles( std::vector<double>& rOutput, const std::vector<double>& GPValues )
{
    //Interpolation of computed values at Lobatto GP to the standard GiD gauss points
    
    rOutput[0] = 0.4905626121623441 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3]
                + 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.009437387837655926 * GPValues[2] + 0.035220810900864506 * GPValues[3];
               
    rOutput[1] = 0.13144585576580212 * GPValues[0] + 0.4905626121623441 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3]
                + 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.009437387837655926 * GPValues[3];
               
    rOutput[2] = 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.4905626121623441 * GPValues[2] + 0.13144585576580212 * GPValues[3]
                + 0.009437387837655926 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3];
               
    rOutput[3] = 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.4905626121623441 * GPValues[3]
                + 0.035220810900864506 * GPValues[0] + 0.009437387837655926 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3];
               
    rOutput[4] = 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.009437387837655926 * GPValues[2] + 0.035220810900864506 * GPValues[3]
                + 0.4905626121623441 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3];
               
    rOutput[5] = 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.009437387837655926 * GPValues[3]
                + 0.13144585576580212 * GPValues[0] + 0.4905626121623441 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3];
               
    rOutput[6] = 0.009437387837655926 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3]
                + 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.4905626121623441 * GPValues[2] + 0.13144585576580212 * GPValues[3];
               
    rOutput[7] = 0.035220810900864506 * GPValues[0] + 0.009437387837655926 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3]
                + 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.4905626121623441 * GPValues[3];
}

//----------------------------------------------------------------------------------------

template<>
template< class TValueType >
void SmallDisplacementInterfaceElement<2,4>::CalculateOutputValues( std::vector<TValueType>& rOutput, const std::vector<TValueType>& GPValues )
{
    //Interpolation of computed values at Lobatto GP to the standard GiD gauss points
    
    noalias(rOutput[0]) = 0.6220084679281462 * GPValues[0] + 0.16666666666666663 * GPValues[1] + 0.044658198738520435 * GPValues[1] + 0.16666666666666663 * GPValues[0];
    
    noalias(rOutput[1]) = 0.16666666666666663 * GPValues[0] + 0.6220084679281462 * GPValues[1] + 0.16666666666666663 * GPValues[1] + 0.044658198738520435 * GPValues[0];
    
    noalias(rOutput[2])= 0.044658198738520435 * GPValues[0] + 0.16666666666666663 * GPValues[1] + 0.6220084679281462 * GPValues[1] + 0.16666666666666663 * GPValues[0];
    
    noalias(rOutput[3]) = 0.16666666666666663 * GPValues[0] + 0.044658198738520435 * GPValues[1] + 0.16666666666666663 * GPValues[1] + 0.6220084679281462 * GPValues[0];
}

//----------------------------------------------------------------------------------------

template<>
template< class TValueType >
void SmallDisplacementInterfaceElement<3,6>::CalculateOutputValues( std::vector<TValueType>& rOutput, const std::vector<TValueType>& GPValues )
{
    //Interpolation of computed values at Lobatto GP to the standard GiD gauss points
    
    noalias(rOutput[0]) = 0.5257834230632086 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.13144585576580214 * GPValues[2]
                        + 0.14088324360345805 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.03522081090086451 * GPValues[2];
               
    noalias(rOutput[1]) = 0.13144585576580214 * GPValues[0] + 0.5257834230632086 * GPValues[1] + 0.13144585576580214 * GPValues[2]
                        + 0.03522081090086451 * GPValues[0] + 0.14088324360345805 * GPValues[1] + 0.03522081090086451 * GPValues[2];
               
    noalias(rOutput[2]) = 0.13144585576580214 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.5257834230632086 * GPValues[2]
                        + 0.03522081090086451 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.14088324360345805 * GPValues[2];
               
    noalias(rOutput[3]) = 0.14088324360345805 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.03522081090086451 * GPValues[2]
                        + 0.5257834230632086 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.13144585576580214 * GPValues[2];
               
    noalias(rOutput[4]) = 0.03522081090086451 * GPValues[0] + 0.14088324360345805 * GPValues[1] + 0.03522081090086451 * GPValues[2]
                        + 0.13144585576580214 * GPValues[0] + 0.5257834230632086 * GPValues[1] + 0.13144585576580214 * GPValues[2];
               
    noalias(rOutput[5]) = 0.03522081090086451 * GPValues[0] + 0.03522081090086451 * GPValues[1] + 0.14088324360345805 * GPValues[2]
                        + 0.13144585576580214 * GPValues[0] + 0.13144585576580214 * GPValues[1] + 0.5257834230632086 * GPValues[2];
}

//----------------------------------------------------------------------------------------

template<>
template< class TValueType >
void SmallDisplacementInterfaceElement<3,8>::CalculateOutputValues( std::vector<TValueType>& rOutput, const std::vector<TValueType>& GPValues )
{
    //Interpolation of computed values at Lobatto GP to the standard GiD gauss points
    
    noalias(rOutput[0]) = 0.4905626121623441 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3]
                        + 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.009437387837655926 * GPValues[2] + 0.035220810900864506 * GPValues[3];
               
    noalias(rOutput[1]) = 0.13144585576580212 * GPValues[0] + 0.4905626121623441 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3]
                        + 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.009437387837655926 * GPValues[3];
               
    noalias(rOutput[2]) = 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.4905626121623441 * GPValues[2] + 0.13144585576580212 * GPValues[3]
                        + 0.009437387837655926 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3];
               
    noalias(rOutput[3]) = 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.4905626121623441 * GPValues[3]
                        + 0.035220810900864506 * GPValues[0] + 0.009437387837655926 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3];
               
    noalias(rOutput[4]) = 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.009437387837655926 * GPValues[2] + 0.035220810900864506 * GPValues[3]
                        + 0.4905626121623441 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3];
               
    noalias(rOutput[5]) = 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.009437387837655926 * GPValues[3]
                        + 0.13144585576580212 * GPValues[0] + 0.4905626121623441 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3];
               
    noalias(rOutput[6]) = 0.009437387837655926 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.035220810900864506 * GPValues[3]
                        + 0.035220810900864506 * GPValues[0] + 0.13144585576580212 * GPValues[1] + 0.4905626121623441 * GPValues[2] + 0.13144585576580212 * GPValues[3];
               
    noalias(rOutput[7]) = 0.035220810900864506 * GPValues[0] + 0.009437387837655926 * GPValues[1] + 0.035220810900864506 * GPValues[2] + 0.13144585576580212 * GPValues[3]
                        + 0.13144585576580212 * GPValues[0] + 0.035220810900864506 * GPValues[1] + 0.13144585576580212 * GPValues[2] + 0.4905626121623441 * GPValues[3];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template class SmallDisplacementInterfaceElement<2,4>;
template class SmallDisplacementInterfaceElement<3,6>;
template class SmallDisplacementInterfaceElement<3,8>;

} // Namespace Kratos
