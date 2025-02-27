
#ifndef KRATOS_TWO_STEP_WERNER_WENGLE_WALL_CONDITION_H
#define KRATOS_TWO_STEP_WERNER_WENGLE_WALL_CONDITION_H

// System includes
#include <iostream>

// External includes

// Project includes
#include "includes/define.h"
#include "includes/serializer.h"
#include "includes/condition.h"
#include "includes/process_info.h"
#include "includes/kratos_flags.h"
#include "includes/deprecated_variables.h"
#include "includes/cfd_variables.h"
 
/* #include "includes/element.h" */


// Application includes
/* #include "pfem_fluid_dynamics_application.h" */

namespace Kratos {
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

/// Implements a power-law wall model.
/**
 The Werner-Wengle wall layer model in "H. Werner and H. Wengle, Large-eddy simulation
 of turbulent flow over and around a cube in a plate channel, 8th Symp. on Turbulent
 Shear Flows 19-4, 1991" is used to calculate wall stress. For distributed problems the
 MetisDivideHeterogeneousInputProcess must be used with SynchronizeConditions = true.
 This ensures the condition belongs to the same partition as its parent element.

 Interface artificial compressibility (IAC) is used on the fluid-structure interface
 to improve the stability of partitioned FSI coupling iterations. This is activated
 by setting the flag INTERFACE to true.

 This element is tested in 3D with and without MPI.

 @see FractionalStep2D, FractionalStep3D
 */
template<unsigned int TDim, unsigned int TNumNodes = TDim>
class TwoStepWernerWengleWallCondition: public Condition
{
public:
	///@name Type Definitions
	///@{

	/// Pointer definition of TwoStepWernerWengleWallCondition
	KRATOS_CLASS_POINTER_DEFINITION(TwoStepWernerWengleWallCondition);

	typedef Node < 3 > NodeType;

	typedef Properties PropertiesType;

	typedef Geometry<NodeType> GeometryType;

	typedef Geometry<NodeType>::PointType PointType;

	typedef Geometry<NodeType>::PointsArrayType NodesArrayType;

	typedef Geometry<NodeType>::GeometriesArrayType GeometriesArrayType;

	typedef Element::WeakPointer ElementWeakPointerType;

	typedef Element::Pointer ElementPointerType;

	typedef Vector VectorType;

	typedef Matrix MatrixType;

	typedef GeometryType::ShapeFunctionsGradientsType ShapeFunctionDerivativesArrayType;

	typedef std::size_t IndexType;

	typedef std::size_t SizeType;

	typedef std::vector<std::size_t> EquationIdVectorType;

	typedef std::vector< Dof<double>::Pointer > DofsVectorType;

	typedef Kratos::Vector ShapeFunctionsType;

	///@}
	///@name Life Cycle
	///@{

	/// Default constructor.
	/** Admits an Id as a parameter.
	 @param NewId Index of the new condition
	 */
	TwoStepWernerWengleWallCondition(IndexType NewId = 0) : Condition(NewId), mInitializeWasPerformed(false), mpElement()
	{
	}

	/// Constructor using an array of nodes.
	/**
	 @param NewId Index of the new condition
	 @param ThisNodes An array containing the nodes of the new condition
	 */
	TwoStepWernerWengleWallCondition(IndexType NewId, const NodesArrayType& ThisNodes)
	: Condition(NewId, ThisNodes), mInitializeWasPerformed(false), mpElement()
	{
	}

	/// Constructor using Geometry.
	/**
	 @param NewId Index of the new condition
	 @param pGeometry Pointer to a geometry object
	 */
	TwoStepWernerWengleWallCondition(IndexType NewId, GeometryType::Pointer pGeometry)
	: Condition(NewId, pGeometry), mInitializeWasPerformed(false), mpElement()
	{
	}

	/// Constructor using Properties.
	/**
	 @param NewId Index of the new condition
	 @param pGeometry Pointer to a geometry object
	 @param pProperties Pointer to the condition's properties
	 */
	TwoStepWernerWengleWallCondition(IndexType NewId, GeometryType::Pointer pGeometry,
			PropertiesType::Pointer pProperties)
	: Condition(NewId, pGeometry, pProperties), mInitializeWasPerformed(false), mpElement()
	{
	}

	/// Copy constructor.
	TwoStepWernerWengleWallCondition(TwoStepWernerWengleWallCondition const& rOther) : Condition(rOther),
	mInitializeWasPerformed(rOther.mInitializeWasPerformed), mpElement(rOther.mpElement)
	{
	}

	/// Destructor.
	virtual ~TwoStepWernerWengleWallCondition()
	{}

	///@}
	///@name Operators
	///@{

	/// Copy constructor.
	TwoStepWernerWengleWallCondition& operator=(TwoStepWernerWengleWallCondition const& rOther)
	{
		Condition::operator=(rOther);
		mInitializeWasPerformed = rOther.mInitializeWasPerformed;
		mpElement = rOther.mpElement;
		return *this;
	}

	///@}
	///@name Operations
	///@{

	/// Create a new TwoStepWernerWengleWallCondition object.
	/**
	 @param NewId Index of the new condition
	 @param ThisNodes An array containing the nodes of the new condition
	 @param pProperties Pointer to the condition's properties
	 */
	virtual Condition::Pointer Create(IndexType NewId,
			NodesArrayType const& ThisNodes,
			PropertiesType::Pointer pProperties) const
	{
		return Condition::Pointer(new TwoStepWernerWengleWallCondition(NewId,
						GetGeometry().Create(ThisNodes), pProperties));
	}

	/// Find the condition's parent element.
	void Initialize()
	{
		KRATOS_TRY;

		const array_1d<double,3>& rNormal = this->GetValue(NORMAL);
		if (norm_2(rNormal) == 0.0)
		  {
		    std::cout << "error on condition -> " << this->Id() << std::endl;
		    KRATOS_THROW_ERROR(std::logic_error, "NORMAL must be calculated before using this condition","");
		  }

		if (mInitializeWasPerformed)
		{
		        return;
		}

		mInitializeWasPerformed = true;

		double EdgeLength;
		array_1d<double,3> Edge;
		GeometryType& rGeom = this->GetGeometry();
		WeakPointerVector<Element> ElementCandidates;
		for (SizeType i = 0; i < TNumNodes; i++)
		{
			WeakPointerVector<Element>& rNodeElementCandidates = rGeom[i].GetValue(NEIGHBOUR_ELEMENTS);
			for (SizeType j = 0; j < rNodeElementCandidates.size(); j++)
			{
				ElementCandidates.push_back(rNodeElementCandidates(j));
			}
		}

		std::vector<IndexType> NodeIds(TNumNodes), ElementNodeIds;

		for (SizeType i=0; i < TNumNodes; i++)
		{
			NodeIds[i] = rGeom[i].Id();
		}

		std::sort(NodeIds.begin(), NodeIds.end());

		for (SizeType i=0; i < ElementCandidates.size(); i++)
		{
			GeometryType& rElemGeom = ElementCandidates[i].GetGeometry();
			ElementNodeIds.resize(rElemGeom.PointsNumber());

			for (SizeType j=0; j < rElemGeom.PointsNumber(); j++)
			{
				ElementNodeIds[j] = rElemGeom[j].Id();
			}

			std::sort(ElementNodeIds.begin(), ElementNodeIds.end());

			if ( std::includes(ElementNodeIds.begin(), ElementNodeIds.end(), NodeIds.begin(), NodeIds.end()) )
			{
				mpElement = ElementCandidates(i);

				Edge = rElemGeom[1].Coordinates() - rElemGeom[0].Coordinates();
				mMinEdgeLength = Edge[0]*Edge[0];
				for (SizeType d=1; d < TDim; d++)
				{
					mMinEdgeLength += Edge[d]*Edge[d];
				}

				for (SizeType j=2; j < rElemGeom.PointsNumber(); j++)
				{
					for(SizeType k=0; k < j; k++)
					{
						Edge = rElemGeom[j].Coordinates() - rElemGeom[k].Coordinates();
						EdgeLength = Edge[0]*Edge[0];

						for (SizeType d = 1; d < TDim; d++)
						{
							EdgeLength += Edge[d]*Edge[d];
						}

						mMinEdgeLength = (EdgeLength < mMinEdgeLength) ? EdgeLength : mMinEdgeLength;
					}
				}
				mMinEdgeLength = sqrt(mMinEdgeLength);
				return;
			}
		}

		std::cout << "error in condition -> " << this->Id() << std::endl;
		KRATOS_THROW_ERROR(std::logic_error, "Condition cannot find parent element","");
		KRATOS_CATCH("");
	}

	virtual void CalculateLeftHandSide(MatrixType& rLeftHandSideMatrix,
			ProcessInfo& rCurrentProcessInfo)
	{
		VectorType RHS;
		this->CalculateLocalSystem(rLeftHandSideMatrix, RHS, rCurrentProcessInfo);
	}

	/// Calculate wall stress term for all nodes with IS_STRUCTURE != 0.
	/**
	 @param rLeftHandSideMatrix Left-hand side matrix
	 @param rRightHandSideVector Right-hand side vector
	 @param rCurrentProcessInfo ProcessInfo instance
	 */
	virtual void CalculateLocalSystem(MatrixType& rLeftHandSideMatrix,
			VectorType& rRightHandSideVector,
			ProcessInfo& rCurrentProcessInfo)
	{
		KRATOS_TRY;

		if (mInitializeWasPerformed == false)
		{
		        Initialize();
		}

		if (rCurrentProcessInfo[FRACTIONAL_STEP] == 1)
		{
			// Initialize local contributions
			const SizeType LocalSize = TDim * TNumNodes;

			if (rLeftHandSideMatrix.size1() != LocalSize)
			{
				rLeftHandSideMatrix.resize(LocalSize, LocalSize);
			}
			if (rRightHandSideVector.size() != LocalSize)
			{
				rRightHandSideVector.resize(LocalSize);
			}

			noalias(rLeftHandSideMatrix) = ZeroMatrix(LocalSize, LocalSize);
			noalias(rRightHandSideVector) = ZeroVector(LocalSize);

			if (this->GetValue(IS_STRUCTURE) != 0.0)
			  this->ApplyWallLaw(rLeftHandSideMatrix, rRightHandSideVector);
		}
		else if (rCurrentProcessInfo[FRACTIONAL_STEP] == 5)
		{
			// add IAC penalty to local pressure system
			const SizeType LocalSize = TNumNodes;

			if (rLeftHandSideMatrix.size1() != LocalSize)
			{
				rLeftHandSideMatrix.resize(LocalSize, LocalSize);
			}
			if (rRightHandSideVector.size() != LocalSize)
			{
				rRightHandSideVector.resize(LocalSize);
			}

			noalias(rLeftHandSideMatrix) = ZeroMatrix(LocalSize, LocalSize);
			noalias(rRightHandSideVector) = ZeroVector(LocalSize);

			if (this->Is(INTERFACE))
			  this->ApplyIACPenalty(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);
		}
		else
		{
			if (rLeftHandSideMatrix.size1() != 0)
			{
				rLeftHandSideMatrix.resize(0,0,false);
			}

			if (rRightHandSideVector.size() != 0)
			{
				rRightHandSideVector.resize(0,false);
			}
		}

		KRATOS_CATCH("");
	}

	/// Check that all data required by this condition is available and reasonable.
	virtual int Check(const ProcessInfo& rCurrentProcessInfo)
	{
		KRATOS_TRY;

		int Check = Condition::Check(rCurrentProcessInfo); // Checks id > 0 and area >= 0

		if (Check != 0)
		{
			return Check;
		}
		else
		{
			// Check that all required variables have been registered
			if(VELOCITY.Key() == 0)
			KRATOS_THROW_ERROR(std::invalid_argument,"VELOCITY Key is 0. Check if the application was correctly registered.","");
			if(PRESSURE.Key() == 0)
			KRATOS_THROW_ERROR(std::invalid_argument,"PRESSURE Key is 0. Check if the application was correctly registered.","");
			if(MESH_VELOCITY.Key() == 0)
			KRATOS_THROW_ERROR(std::invalid_argument,"MESH_VELOCITY Key is 0. Check if the application was correctly registered.","");
			if(DENSITY.Key() == 0)
			KRATOS_THROW_ERROR(std::invalid_argument,"DENSITY Key is 0. Check if the application was correctly registered.","");
			if(VISCOSITY.Key() == 0)
			KRATOS_THROW_ERROR(std::invalid_argument,"VISCOSITY Key is 0. Check if the application was correctly registered.","");
			if(NORMAL.Key() == 0)
			KRATOS_THROW_ERROR(std::invalid_argument,"NORMAL Key is 0. Check if the application was correctly registered.","");
			if(IS_STRUCTURE.Key() == 0)
			KRATOS_THROW_ERROR(std::invalid_argument,"IS_STRUCTURE Key is 0. Check if the application was correctly registered.","");

			// Check that the element's nodes contain all required SolutionStepData and Degrees of freedom
			for(unsigned int i=0; i<this->GetGeometry().size(); ++i)
			{
				if(this->GetGeometry()[i].SolutionStepsDataHas(VELOCITY) == false)
				KRATOS_THROW_ERROR(std::invalid_argument,"missing VELOCITY variable on solution step data for node ",this->GetGeometry()[i].Id());
				if(this->GetGeometry()[i].SolutionStepsDataHas(MESH_VELOCITY) == false)
				KRATOS_THROW_ERROR(std::invalid_argument,"missing MESH_VELOCITY variable on solution step data for node ",this->GetGeometry()[i].Id());
				if(this->GetGeometry()[i].SolutionStepsDataHas(DENSITY) == false)
				KRATOS_THROW_ERROR(std::invalid_argument,"missing DENSITY variable on solution step data for node ",this->GetGeometry()[i].Id());
				if(this->GetGeometry()[i].SolutionStepsDataHas(VISCOSITY) == false)
				KRATOS_THROW_ERROR(std::invalid_argument,"missing VISCOSITY variable on solution step data for node ",this->GetGeometry()[i].Id());
				if(this->GetGeometry()[i].SolutionStepsDataHas(NORMAL) == false)
				KRATOS_THROW_ERROR(std::invalid_argument,"missing NORMAL variable on solution step data for node ",this->GetGeometry()[i].Id());
				if(this->GetGeometry()[i].HasDofFor(VELOCITY_X) == false ||
						this->GetGeometry()[i].HasDofFor(VELOCITY_Y) == false ||
						this->GetGeometry()[i].HasDofFor(VELOCITY_Z) == false)
				KRATOS_THROW_ERROR(std::invalid_argument,"missing VELOCITY component degree of freedom on node ",this->GetGeometry()[i].Id());
				if(this->GetGeometry()[i].HasDofFor(PRESSURE) == false)
				KRATOS_THROW_ERROR(std::invalid_argument,"missing PRESSURE degree of freedom on node ",this->GetGeometry()[i].Id());
			}

			return Check;
		}

		KRATOS_CATCH("");
	}

	/// Provides the global indices for each one of this condition's local rows.
	/** This determines the equation ID vector for all DOFs.
	 * @param rResult A vector containing the global Id of each row
	 * @param rCurrentProcessInfo the current process info object
	 */
	virtual void EquationIdVector(EquationIdVectorType& rResult,
			ProcessInfo& rCurrentProcessInfo);

	/// Returns a list of the condition's Dofs.
	/**
	 * @param ConditionDofList the list of DOFs
	 * @param rCurrentProcessInfo the current process info instance
	 */
	virtual void GetDofList(DofsVectorType& rConditionDofList,
			ProcessInfo& rCurrentProcessInfo);

	/// Returns VELOCITY_X, VELOCITY_Y, (VELOCITY_Z) for each node.
	/**
	 * @param Values Vector of nodal unknowns
	 * @param Step Get result from 'Step' steps back, 0 is current step. (Must be smaller than buffer size)
	 */
	virtual void GetValuesVector(Vector& Values, int Step = 0)
	{
		const SizeType LocalSize = TDim * TNumNodes;
		unsigned int LocalIndex = 0;

		if (Values.size() != LocalSize)
		{
			Values.resize(LocalSize, false);
		}

		for (unsigned int i = 0; i < TNumNodes; ++i)
		{
			array_1d<double,3>& rVelocity = this->GetGeometry()[i].FastGetSolutionStepValue(VELOCITY, Step);
			for (unsigned int d = 0; d < TDim; ++d)
			{
				Values[LocalIndex++] = rVelocity[d];
			}
		}
	}

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
		std::stringstream buffer;
		buffer << "TwoStepWernerWengleWallCondition" << TDim << "D";
		return buffer.str();
	}

	/// Print information about this object.
	virtual void PrintInfo(std::ostream& rOStream) const
	{	rOStream << "TwoStepWernerWengleWallCondition";}

	/// Print object's data.
	virtual void PrintData(std::ostream& rOStream) const
	{}

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

	ElementPointerType pGetElement()
	{
		return mpElement.lock();
	}

	template< class TVariableType >
	void EvaluateInPoint(TVariableType& rResult,
			const Kratos::Variable<TVariableType>& Var,
			const ShapeFunctionsType& rShapeFunc)
	{
		GeometryType& rGeom = this->GetGeometry();

		rResult = rShapeFunc[0] * rGeom[0].FastGetSolutionStepValue(Var);

		for(SizeType i = 1; i < TNumNodes; i++)
		{
			rResult += rShapeFunc[i] * rGeom[i].FastGetSolutionStepValue(Var);
		}
	}

	/// Calculate input parameters to wall model.
	/**
	 * @param rWallHeight The height of the measurement point above the wall
	 * @param rWallVel The tangential velocity vector at the measurement point
	 * @param rArea The condition's area
	 */
	void CalculateWallParameters(double& rWallHeight, array_1d<double,3>& rWallVel, double& rArea);

	/// Compute the wall stress and add corresponding terms to the system contributions.
	/**
	 @param rLocalMatrix Local system matrix
	 @param rLocalVector Local right hand side
	 */
	void ApplyWallLaw(MatrixType& rLocalMatrix, VectorType& rLocalVector)
	{
		const double A = 8.3;
		const double Alpha = 1.0 / 7.0;
		const double Small = 1.0e-12;
		const unsigned int BlockSize = TDim;
		double WallHeight, Area, rho, nu, WallVelMag, WallVelCut, tmp, WallStress, WallForce;

		array_1d<double,3> WallVel;
		GeometryType& rGeometry = this->GetGeometry();
		CalculateWallParameters(WallHeight, WallVel, Area);
		WallHeight = (WallHeight > Small * mMinEdgeLength) ? WallHeight : Small * mMinEdgeLength;
		WallVelMag = norm_2(WallVel);

		if (WallVelMag > Small)
		{
			const ShapeFunctionsType& N = row(this->GetGeometry().ShapeFunctionsValues(GeometryData::GI_GAUSS_1),0);
			EvaluateInPoint(rho, DENSITY, N);
			EvaluateInPoint(nu, VISCOSITY, N);

			WallVelCut = nu * pow(A, 2.0/(1.0-Alpha)) / (2.0 * WallHeight);

			// linear region
			if (WallVelMag <= WallVelCut)
			{
				WallStress = 2.0 * rho * nu * WallVelMag / WallHeight;
			}
			else
			{ // log region
				tmp = (1.0 - Alpha) / 2.0 * pow(A, (1.0 + Alpha) / (1.0 - Alpha)) * pow(nu / WallHeight, 1.0 + Alpha);
				tmp += (1.0 + Alpha) / A * pow(nu / WallHeight, Alpha) * WallVelMag;
				WallStress = rho * pow(tmp, 2.0 / (1.0 + Alpha));
			}

			WallForce = (Area / static_cast<double>(TNumNodes)) * WallStress;

			for(SizeType i=0; i < rGeometry.PointsNumber(); ++i)
			{
				const NodeType& rNode = rGeometry[i];
				if(rNode.GetValue(Y_WALL) != 0.0 && rNode.GetValue(IS_STRUCTURE) != 0.0)
				{
					WallVel = rNode.FastGetSolutionStepValue(VELOCITY,1) - rNode.FastGetSolutionStepValue(MESH_VELOCITY,1);
					tmp = norm_2(WallVel);
					WallVel /= (tmp > Small) ? tmp : 1.0;

					for (unsigned int d=0; d < TDim; d++)
					{
						unsigned int k = i * BlockSize + d;
						rLocalVector[k] -= WallVel[d] * WallForce;
					}
				}
			}
		}
	}

	/// Apply an IAC penalty term
	/**
	 @param rLeftHandSideMatrix Left-hand side matrix
	 @param rRightHandSideVector Right-hand side vector
	 @param rCurrentProcessInfo ProcessInfo instance
	 */
	void ApplyIACPenalty(MatrixType& rLeftHandSideMatrix,
			VectorType& rRightHandSideVector,
			ProcessInfo& rCurrentProcessInfo)
	{
		GeometryType& rGeometry = this->GetGeometry();
		const array_1d<double,3>& rNormal = this->GetValue(NORMAL);
		const double Area = norm_2(rNormal);
		const double DiagonalTerm = Area / static_cast<double>(TNumNodes)
		/ (rCurrentProcessInfo[DENSITY] * rCurrentProcessInfo[BDF_COEFFICIENTS][0]);

		for(SizeType iNode=0; iNode < rGeometry.PointsNumber(); ++iNode)
		{
			rLeftHandSideMatrix(iNode,iNode) += DiagonalTerm;
		}
	}

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
	bool mInitializeWasPerformed;
	double mMinEdgeLength;
	ElementWeakPointerType mpElement;

	///@}
	///@name Serialization
	///@{

	friend class Serializer;

	virtual void save(Serializer& rSerializer) const
	{
		KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Condition );
	}

	virtual void load(Serializer& rSerializer)
	{
		KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Condition );
	}

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

	///@}

}; // Class TwoStepWernerWengleWallCondition

///@}

///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

/// input stream function
template<unsigned int TDim, unsigned int TNumNodes>
inline std::istream& operator >>(std::istream& rIStream,
		TwoStepWernerWengleWallCondition<TDim, TNumNodes>& rThis)
{
	return rIStream;
}

/// output stream function
template<unsigned int TDim, unsigned int TNumNodes>
inline std::ostream& operator <<(std::ostream& rOStream,
		const TwoStepWernerWengleWallCondition<TDim, TNumNodes>& rThis)
{
	rThis.PrintInfo(rOStream);
	rOStream << std::endl;
	rThis.PrintData(rOStream);

	return rOStream;
}

///@}

///@} addtogroup block

}// namespace Kratos.

#endif // KRATOS_TWO_STEP_WERNER_WENGLE_WALL_CONDITION_H
