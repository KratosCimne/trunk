//
//   Project Name:        KratosPfemSolidMechanicsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                    July 2013 $
//   Revision:            $Revision:                      0.0 $
//
//

// System includes


// External includes


// Project includes
#include "custom_conditions/point_rigid_contact_condition.hpp"

#include "pfem_solid_mechanics_application_variables.h"

#include "custom_conditions/custom_friction_laws/coulomb_adhesion_friction_law.hpp"
#include "custom_conditions/custom_friction_laws/hardening_coulomb_friction_law.hpp"

namespace Kratos
{

   /**
    * Flags related to the condition computation
    */
   KRATOS_CREATE_LOCAL_FLAG( PointRigidContactCondition, COMPUTE_RHS_VECTOR,                 0 );
   KRATOS_CREATE_LOCAL_FLAG( PointRigidContactCondition, COMPUTE_LHS_MATRIX,                 1 );
   KRATOS_CREATE_LOCAL_FLAG( PointRigidContactCondition, COMPUTE_RHS_VECTOR_WITH_COMPONENTS, 2 );
   KRATOS_CREATE_LOCAL_FLAG( PointRigidContactCondition, COMPUTE_LHS_MATRIX_WITH_COMPONENTS, 3 );


   //***********************************************************************************
   //***********************************************************************************
   PointRigidContactCondition::PointRigidContactCondition(IndexType NewId, GeometryType::Pointer pGeometry)
      : Condition(NewId, pGeometry)
   {
      //DO NOT ADD DOFS HERE!!!
   }

   //***********************************************************************************
   //***********************************************************************************
   PointRigidContactCondition::PointRigidContactCondition(IndexType NewId, GeometryType::Pointer pGeometry, PropertiesType::Pointer pProperties)
      : Condition(NewId, pGeometry, pProperties)
   {

      mThisIntegrationMethod = GetGeometry().GetDefaultIntegrationMethod();
      mpFrictionLaw = ContactFrictionLaw::Pointer( new CoulombAdhesionFrictionLaw() );
      //mpFrictionLaw = ContactFrictionLaw::Pointer( new HardeningCoulombFrictionLaw() );
      //DO NOT ADD DOFS HERE!!!
   }


   //************************************************************************************
   //************************************************************************************
   PointRigidContactCondition::PointRigidContactCondition(IndexType NewId, GeometryType::Pointer pGeometry,  PropertiesType::Pointer pProperties, SpatialBoundingBox::Pointer pRigidWall)
      : Condition(NewId, pGeometry, pProperties)
   {

      mThisIntegrationMethod = GetGeometry().GetDefaultIntegrationMethod();
      mpRigidWall = pRigidWall;
      mpFrictionLaw = ContactFrictionLaw::Pointer( new CoulombAdhesionFrictionLaw() );
      //mpFrictionLaw = ContactFrictionLaw::Pointer( new HardeningCoulombFrictionLaw() );

      //DO NOT ADD DOFS HERE!!!
   }


   //************************************************************************************
   //************************************************************************************
   PointRigidContactCondition::PointRigidContactCondition( PointRigidContactCondition const& rOther )
      : Condition(rOther)
      ,mThisIntegrationMethod(rOther.mThisIntegrationMethod)
      ,mpRigidWall(rOther.mpRigidWall)

   {
      mpFrictionLaw = ContactFrictionLaw::Pointer( new CoulombAdhesionFrictionLaw() );
      //mpFrictionLaw = ContactFrictionLaw::Pointer( new HardeningCoulombFrictionLaw() );
   }

   //***********************************************************************************
   //***********************************************************************************
   Condition::Pointer PointRigidContactCondition::Create(
         IndexType NewId,
         NodesArrayType const& ThisNodes,
         PropertiesType::Pointer pProperties) const
   {
      return Condition::Pointer(new PointRigidContactCondition(NewId, GetGeometry().Create(ThisNodes), pProperties));
   }


   //************************************CLONE*******************************************
   //************************************************************************************

   Condition::Pointer PointRigidContactCondition::Clone( IndexType NewId, NodesArrayType const& rThisNodes ) const
   {
      return this->Create( NewId, rThisNodes, pGetProperties() );
   }


   //***********************************************************************************
   //***********************************************************************************
   PointRigidContactCondition::~PointRigidContactCondition()
   {
   }

   //************* GETTING METHODS

   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::GetDofList(DofsVectorType& rConditionDofList,
         ProcessInfo& rCurrentProcessInfo)
   {
      KRATOS_TRY

      rConditionDofList.resize(0);
      const unsigned int number_of_nodes = GetGeometry().PointsNumber();
      const unsigned int dimension = GetGeometry().WorkingSpaceDimension();

      for (unsigned int i = 0; i < number_of_nodes; i++)
      {
         rConditionDofList.push_back(GetGeometry()[i].pGetDof(DISPLACEMENT_X));
         rConditionDofList.push_back(GetGeometry()[i].pGetDof(DISPLACEMENT_Y));
         if( dimension == 3 )
            rConditionDofList.push_back(GetGeometry()[i].pGetDof(DISPLACEMENT_Z));
      }


      KRATOS_CATCH( "" )
   }

   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::EquationIdVector(EquationIdVectorType& rResult,
         ProcessInfo& rCurrentProcessInfo)
   {
      KRATOS_TRY

      const unsigned int number_of_nodes = GetGeometry().PointsNumber();
      const unsigned int dimension       = GetGeometry().WorkingSpaceDimension();
      unsigned int condition_size        = number_of_nodes * dimension;

      if (rResult.size() != condition_size)
         rResult.resize( condition_size, false );

      for (unsigned int i = 0; i < number_of_nodes; i++)
      {
         int index = i * dimension;
         rResult[index]     = GetGeometry()[i].GetDof(DISPLACEMENT_X).EquationId();
         rResult[index + 1] = GetGeometry()[i].GetDof(DISPLACEMENT_Y).EquationId();
         if( dimension == 3)
            rResult[index + 2] = GetGeometry()[i].GetDof(DISPLACEMENT_Z).EquationId();
      }

      KRATOS_CATCH( "" )
   }


   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::GetValuesVector(Vector& rValues, int Step)
   {
      const unsigned int number_of_nodes = GetGeometry().PointsNumber();
      const unsigned int dimension       = GetGeometry().WorkingSpaceDimension();
      unsigned int       condition_size  = number_of_nodes * dimension;

      if ( rValues.size() != condition_size ) 
         rValues.resize( condition_size, false );

      for (unsigned int i = 0; i < number_of_nodes; i++)
      {
         unsigned int index = i * dimension;
         rValues[index]     = GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT_X, Step );
         rValues[index + 1] = GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT_Y, Step );

         if ( dimension == 3 )
            rValues[index + 2] = GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT_Z, Step );
      }
   }

   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::GetFirstDerivativesVector( Vector& rValues, int Step )
   {
      const unsigned int number_of_nodes = GetGeometry().PointsNumber();
      const unsigned int dimension       = GetGeometry().WorkingSpaceDimension();
      unsigned int       condition_size    = number_of_nodes * dimension;

      if ( rValues.size() != condition_size ) rValues.resize( condition_size, false );

      for ( unsigned int i = 0; i < number_of_nodes; i++ )
      {
         unsigned int index = i * dimension;
         rValues[index]     = GetGeometry()[i].GetSolutionStepValue( VELOCITY_X, Step );
         rValues[index + 1] = GetGeometry()[i].GetSolutionStepValue( VELOCITY_Y, Step );

         if ( dimension == 3 )
            rValues[index + 2] = GetGeometry()[i].GetSolutionStepValue( VELOCITY_Z, Step );
      }
   }


   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::GetSecondDerivativesVector( Vector& rValues, int Step )
   {
      const unsigned int number_of_nodes = GetGeometry().PointsNumber();
      const unsigned int dimension       = GetGeometry().WorkingSpaceDimension();
      unsigned int       condition_size    = number_of_nodes * dimension;

      if ( rValues.size() != condition_size ) rValues.resize( condition_size, false );

      for ( unsigned int i = 0; i < number_of_nodes; i++ )
      {
         unsigned int index = i * dimension;
         rValues[index]     = GetGeometry()[i].GetSolutionStepValue( ACCELERATION_X, Step );
         rValues[index + 1] = GetGeometry()[i].GetSolutionStepValue( ACCELERATION_Y, Step );

         if ( dimension == 3 )
            rValues[index + 2] = GetGeometry()[i].GetSolutionStepValue( ACCELERATION_Z, Step );
      }

   }


   //************************************************************************************
   //************************************************************************************
   void PointRigidContactCondition::ClearNodalForces()
   {
      KRATOS_TRY

      const unsigned int number_of_nodes = GetGeometry().PointsNumber();

      for ( unsigned int i = 0; i < number_of_nodes; i++ )
      {
         GetGeometry()[i].SetLock();
         array_1d<double, 3> & ContactForce  = GetGeometry()[i].FastGetSolutionStepValue(CONTACT_FORCE);
	 //std::cout<<" ContactForce["<<GetGeometry()[i].Id()<<"] "<<ContactForce<<std::endl;
         ContactForce.clear();
         GetGeometry()[i].UnSetLock();
      }

      //KRATOS_WATCH( " CLEAR NODAL FORCE " )

      KRATOS_CATCH( "" )
   }

   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::AddExplicitContribution(const VectorType& rRHSVector, 
         const Variable<VectorType>& rRHSVariable, 
         Variable<array_1d<double,3> >& rDestinationVariable, 
         const ProcessInfo& rCurrentProcessInfo)
   {
      KRATOS_TRY

      const unsigned int number_of_nodes = GetGeometry().PointsNumber();
      const unsigned int dimension       = GetGeometry().WorkingSpaceDimension();

      if( rRHSVariable == CONTACT_FORCES_VECTOR && rDestinationVariable == CONTACT_FORCE )
      {

         for(unsigned int i=0; i< number_of_nodes; i++)
         {
            int index = dimension * i;

            GetGeometry()[i].SetLock();

            array_1d<double, 3 > &ContactForce = GetGeometry()[i].FastGetSolutionStepValue(CONTACT_FORCE);
            for(unsigned int j=0; j<dimension; j++)
            {
               ContactForce[j] += rRHSVector[index + j];
            }

            GetGeometry()[i].UnSetLock();
         }
      }


      if( rRHSVariable == RESIDUAL_VECTOR && rDestinationVariable == FORCE_RESIDUAL )
      {

         for(unsigned int i=0; i< number_of_nodes; i++)
         {
            int index = dimension * i;

            GetGeometry()[i].SetLock();

            array_1d<double, 3 > &ForceResidual = GetGeometry()[i].FastGetSolutionStepValue(FORCE_RESIDUAL);
            for(unsigned int j=0; j<dimension; j++)
            {
               ForceResidual[j] += rRHSVector[index + j];
            }

            GetGeometry()[i].UnSetLock();
         }
      }

      KRATOS_CATCH( "" )
   }

   //************* STARTING - ENDING  METHODS
   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::InitializeSolutionStep( ProcessInfo& rCurrentProcessInfo )
   {
      KRATOS_TRY

      //when implex is active --> it deletes contact forces at last implex step<--
      //ClearNodalForces();

      KRATOS_CATCH( "" )
   }


   //************************************************************************************
   //************************************************************************************
   void PointRigidContactCondition::InitializeNonLinearIteration(ProcessInfo& CurrentProcessInfo)
   {
      CurrentProcessInfo[NUMBER_OF_ACTIVE_CONTACTS] = 0;
      CurrentProcessInfo[NUMBER_OF_STICK_CONTACTS]  = 0;
      CurrentProcessInfo[NUMBER_OF_SLIP_CONTACTS]   = 0;

      ClearNodalForces();
   }

   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::FinalizeNonLinearIteration(ProcessInfo& CurrentProcessInfo)
   {
   }

   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::FinalizeSolutionStep( ProcessInfo& CurrentProcessInfo )
   {
      KRATOS_TRY

      // CurrentProcessInfo[NUMBER_OF_ACTIVE_CONTACTS] = 0;
      // CurrentProcessInfo[NUMBER_OF_STICK_CONTACTS]  = 0;
      // CurrentProcessInfo[NUMBER_OF_SLIP_CONTACTS]   = 0;

      KRATOS_CATCH( "" )
   }
   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::InitializeSystemMatrices(MatrixType& rLeftHandSideMatrix,
         VectorType& rRightHandSideVector,
         Flags& rCalculationFlags)

   {

      const unsigned int number_of_nodes = GetGeometry().PointsNumber();
      const unsigned int dimension       = GetGeometry().WorkingSpaceDimension();

      //resizing as needed the LHS
      unsigned int MatSize = number_of_nodes * dimension;

      if ( rCalculationFlags.Is(PointRigidContactCondition::COMPUTE_LHS_MATRIX) ) //calculation of the matrix is required
      {
         if ( rLeftHandSideMatrix.size1() != MatSize )
            rLeftHandSideMatrix.resize( MatSize, MatSize, false );

         noalias( rLeftHandSideMatrix ) = ZeroMatrix( MatSize, MatSize ); //resetting LHS
      }


      //resizing as needed the RHS
      if ( rCalculationFlags.Is(PointRigidContactCondition::COMPUTE_RHS_VECTOR) ) //calculation of the matrix is required
      {
         if ( rRightHandSideVector.size() != MatSize )
            rRightHandSideVector.resize( MatSize, false );

         rRightHandSideVector = ZeroVector( MatSize ); //resetting RHS

      }
   }


   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::InitializeGeneralVariables(GeneralVariables& rVariables, const ProcessInfo& rCurrentProcessInfo)
   {
      KRATOS_TRY


      KRATOS_CATCH( "" )

   }

   //*********************************COMPUTE KINEMATICS*********************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateKinematics(GeneralVariables& rVariables,
         const double& rPointNumber)
   {
      KRATOS_TRY

      KRATOS_THROW_ERROR( std::logic_error, "calling the default CalculateKinematics method for a force load condition ... illegal operation!!", "" )

      KRATOS_CATCH( "" )
   }


   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateConditionSystem(LocalSystemComponents& rLocalSystem,
         ProcessInfo& rCurrentProcessInfo)
   {
      KRATOS_TRY

      //create and initialize condition variables:
      GeneralVariables Variables;
      this->InitializeGeneralVariables(Variables,rCurrentProcessInfo);

      //reading integration points
      for ( unsigned int PointNumber = 0; PointNumber < 1; PointNumber++ )
      {
         //compute element kinematics B, F, DN_DX ...
         this->CalculateKinematics(Variables,PointNumber);

         //calculating weights for integration on the "reference configuration"
         double IntegrationWeight = 1;
         IntegrationWeight = this->CalculateIntegrationWeight( IntegrationWeight );

 	 if ( rLocalSystem.CalculationFlags.Is(PointRigidContactCondition::COMPUTE_LHS_MATRIX) ) //calculation of the matrix is required
	   {
	     //contributions to stiffness matrix calculated on the reference config
	     this->CalculateAndAddLHS ( rLocalSystem, Variables, IntegrationWeight );
	   }

	 if ( rLocalSystem.CalculationFlags.Is(PointRigidContactCondition::COMPUTE_RHS_VECTOR) ) //calculation of the vector is required
	   {
	     //contribution to external forces 
	     this->CalculateAndAddRHS ( rLocalSystem, Variables, IntegrationWeight );
	   }
	
         if( Variables.Options.Is(ACTIVE) )// {
	   rCurrentProcessInfo[NUMBER_OF_ACTIVE_CONTACTS] += 1; 
	 //   std::cout<<" ACTIVE_CONTACTS "<<rCurrentProcessInfo[NUMBER_OF_ACTIVE_CONTACTS]<<std::endl;
	 //   std::cout<<" RHS "<<rLocalSystem.GetRightHandSideVector()<<std::endl;
	 // }
	 // else{
	 //   std::cout<<" NON Active Contact "<<GetGeometry()[0].Id()<<" "<<Variables.Gap.Normal<<" normal"<<Variables.Surface.Normal<<" "<<GetGeometry()[0].Coordinates()<<std::endl;
	 // }
      }

      KRATOS_CATCH( "" )
   }


   //************* COMPUTING  METHODS
   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateAndAddLHS(LocalSystemComponents& rLocalSystem, GeneralVariables& rVariables, double& rIntegrationWeight)
   {

      //contributions of the stiffness matrix calculated on the reference configuration
      if( rLocalSystem.CalculationFlags.Is( PointRigidContactCondition::COMPUTE_LHS_MATRIX_WITH_COMPONENTS ) )
      {
         std::vector<MatrixType>& rLeftHandSideMatrices = rLocalSystem.GetLeftHandSideMatrices();
         const std::vector< Variable< MatrixType > >& rLeftHandSideVariables = rLocalSystem.GetLeftHandSideVariables();

         for( unsigned int i=0; i<rLeftHandSideVariables.size(); i++ )
         {
            bool calculated = false;

            if( rLeftHandSideVariables[i] == GEOMETRIC_STIFFNESS_MATRIX ){
               // operation performed: add Kg to the rLefsHandSideMatrix
               this->CalculateAndAddKuug( rLeftHandSideMatrices[i], rVariables, rIntegrationWeight );
               calculated = true;
            }

            if(calculated == false)
            {
               KRATOS_THROW_ERROR(std::logic_error, " ELEMENT can not supply the required local system variable: ",rLeftHandSideVariables[i])
            }

         }
      } 
      else{

         MatrixType& rLeftHandSideMatrix = rLocalSystem.GetLeftHandSideMatrix(); 

         // operation performed: add Kg to the rLefsHandSideMatrix
         this->CalculateAndAddKuug( rLeftHandSideMatrix, rVariables, rIntegrationWeight );

         //KRATOS_WATCH( rLeftHandSideMatrix )
      }

   }


   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateAndAddRHS(LocalSystemComponents& rLocalSystem, GeneralVariables& rVariables, double& rIntegrationWeight)
   {
      //contribution of the internal and external forces
      if( rLocalSystem.CalculationFlags.Is( PointRigidContactCondition::COMPUTE_RHS_VECTOR_WITH_COMPONENTS ) )
      {

         std::vector<VectorType>& rRightHandSideVectors = rLocalSystem.GetRightHandSideVectors();
         const std::vector< Variable< VectorType > >& rRightHandSideVariables = rLocalSystem.GetRightHandSideVariables();
         for( unsigned int i=0; i<rRightHandSideVariables.size(); i++ )
         {
            bool calculated = false;
            if( rRightHandSideVariables[i] == CONTACT_FORCES_VECTOR ){
               // operation performed: rRightHandSideVector += ExtForce*IntToReferenceWeight
               this->CalculateAndAddContactForces( rRightHandSideVectors[i], rVariables, rIntegrationWeight );
               calculated = true;
            }

            if(calculated == false)
            {
               KRATOS_THROW_ERROR(std::logic_error, " ELEMENT can not supply the required local system variable: ",rRightHandSideVariables[i])
            }

         }
      }
      else{

         VectorType& rRightHandSideVector = rLocalSystem.GetRightHandSideVector(); 

         // operation performed: rRightHandSideVector += ExtForce*IntToReferenceWeight
         this->CalculateAndAddContactForces( rRightHandSideVector, rVariables, rIntegrationWeight );

         //KRATOS_WATCH( rRightHandSideVector )

      }


   }

   //***********************************************************************************
   //************************************************************************************

   double& PointRigidContactCondition::CalculateIntegrationWeight(double& rIntegrationWeight)
   {
      return rIntegrationWeight;
   }


   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateRightHandSide( VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo )
   {
      //create local system components
      LocalSystemComponents LocalSystem;

      //calculation flags
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_RHS_VECTOR);

      MatrixType LeftHandSideMatrix = Matrix();

      //Initialize sizes for the system components:
      this->InitializeSystemMatrices( LeftHandSideMatrix, rRightHandSideVector, LocalSystem.CalculationFlags );

      //Set Variables to Local system components
      LocalSystem.SetLeftHandSideMatrix(LeftHandSideMatrix);
      LocalSystem.SetRightHandSideVector(rRightHandSideVector);

      //Calculate condition system
      this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );

   }

   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateRightHandSide( std::vector< VectorType >& rRightHandSideVectors, const std::vector< Variable< VectorType > >& rRHSVariables, ProcessInfo& rCurrentProcessInfo )
   {
      //create local system components
      LocalSystemComponents LocalSystem;

      //calculation flags
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_RHS_VECTOR);
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_RHS_VECTOR_WITH_COMPONENTS);

      MatrixType LeftHandSideMatrix = Matrix();

      //Initialize sizes for the system components:
      if( rRHSVariables.size() != rRightHandSideVectors.size() )
         rRightHandSideVectors.resize(rRHSVariables.size());

      for( unsigned int i=0; i<rRightHandSideVectors.size(); i++ )
      {
         this->InitializeSystemMatrices( LeftHandSideMatrix, rRightHandSideVectors[i], LocalSystem.CalculationFlags );
      }

      //Set Variables to Local system components
      LocalSystem.SetLeftHandSideMatrix(LeftHandSideMatrix);
      LocalSystem.SetRightHandSideVectors(rRightHandSideVectors);

      LocalSystem.SetRightHandSideVariables(rRHSVariables);

      //Calculate condition system
      this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );


   }



   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateLocalSystem( MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo )
   {
      //create local system components
      LocalSystemComponents LocalSystem;

      //calculation flags
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_LHS_MATRIX);
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_RHS_VECTOR);

      //Initialize sizes for the system components:
      this->InitializeSystemMatrices( rLeftHandSideMatrix, rRightHandSideVector, LocalSystem.CalculationFlags );

      //Set Variables to Local system components
      LocalSystem.SetLeftHandSideMatrix(rLeftHandSideMatrix);
      LocalSystem.SetRightHandSideVector(rRightHandSideVector);

      //Calculate condition system
      this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );

      //std::cout<<" ContactID ["<<this->Id()<<"]: LHS "<<rLeftHandSideMatrix<<std::endl;
      //std::cout<<" ContactID ["<<this->Id()<<"]: RHS "<<rRightHandSideVector<<std::endl;
   }


   //************************************************************************************
   //************************************************************************************

   void PointRigidContactCondition::CalculateLocalSystem( std::vector< MatrixType >& rLeftHandSideMatrices,
         const std::vector< Variable< MatrixType > >& rLHSVariables,
         std::vector< VectorType >& rRightHandSideVectors,
         const std::vector< Variable< VectorType > >& rRHSVariables,
         ProcessInfo& rCurrentProcessInfo )
   {
      //create local system components
      LocalSystemComponents LocalSystem;

      //calculation flags
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_LHS_MATRIX_WITH_COMPONENTS);
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_RHS_VECTOR_WITH_COMPONENTS);


      //Initialize sizes for the system components:
      if( rLHSVariables.size() != rLeftHandSideMatrices.size() )
         rLeftHandSideMatrices.resize(rLHSVariables.size());

      if( rRHSVariables.size() != rRightHandSideVectors.size() )
         rRightHandSideVectors.resize(rRHSVariables.size());

      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_LHS_MATRIX);
      for( unsigned int i=0; i<rLeftHandSideMatrices.size(); i++ )
      {
         //Note: rRightHandSideVectors.size() > 0
         this->InitializeSystemMatrices( rLeftHandSideMatrices[i], rRightHandSideVectors[0], LocalSystem.CalculationFlags );
      }

      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_RHS_VECTOR);
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_LHS_MATRIX,false);

      for( unsigned int i=0; i<rRightHandSideVectors.size(); i++ )
      {
         //Note: rLeftHandSideMatrices.size() > 0
         this->InitializeSystemMatrices( rLeftHandSideMatrices[0], rRightHandSideVectors[i], LocalSystem.CalculationFlags );
      }
      LocalSystem.CalculationFlags.Set(PointRigidContactCondition::COMPUTE_LHS_MATRIX,true);


      //Set Variables to Local system components
      LocalSystem.SetLeftHandSideMatrices(rLeftHandSideMatrices);
      LocalSystem.SetRightHandSideVectors(rRightHandSideVectors);

      LocalSystem.SetLeftHandSideVariables(rLHSVariables);
      LocalSystem.SetRightHandSideVariables(rRHSVariables);

      //Calculate condition system
      this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );

   }


   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::CalculateMassMatrix( MatrixType& rMassMatrix, ProcessInfo& rCurrentProcessInfo)
   {
      KRATOS_TRY

      rMassMatrix.resize(0, 0, false);

      KRATOS_CATCH( "" )
   }

   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::CalculateDampingMatrix( MatrixType& rDampingMatrix, ProcessInfo& rCurrentProcessInfo)
   {
      KRATOS_TRY

      rDampingMatrix.resize(0, 0, false);

      KRATOS_CATCH( "" )
   }


   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::CalculateAndAddKuug(MatrixType& rLeftHandSideMatrix,
         GeneralVariables& rVariables,
         double& rIntegrationWeight)

   {
      KRATOS_TRY



      KRATOS_CATCH( "" )
   }


   //***********************************************************************************
   //***********************************************************************************

   void PointRigidContactCondition::CalculateAndAddContactForces(VectorType& rRightHandSideVector,
         GeneralVariables& rVariables,
         double& rIntegrationWeight)

   {
      KRATOS_TRY


      KRATOS_CATCH( "" )
   }


   //***********************************************************************************
   //***********************************************************************************


   int PointRigidContactCondition::Check( const ProcessInfo& rCurrentProcessInfo )
   {
      return 0;
   }

   //***********************************************************************************
   //***********************************************************************************

} // Namespace Kratos.
