#include "two_step_periodic_condition.h"

namespace Kratos
{

template< unsigned int TDim >
TwoStepPeriodicCondition<TDim>::TwoStepPeriodicCondition(IndexType NewId):
    Condition(NewId)
{
}

template< unsigned int TDim >
TwoStepPeriodicCondition<TDim>::TwoStepPeriodicCondition(IndexType NewId, const NodesArrayType& ThisNodes):
    Condition(NewId,ThisNodes)
{
}

template< unsigned int TDim >
TwoStepPeriodicCondition<TDim>::TwoStepPeriodicCondition(IndexType NewId, GeometryType::Pointer pGeometry):
    Condition(NewId,pGeometry)
{
}

template< unsigned int TDim >
TwoStepPeriodicCondition<TDim>::TwoStepPeriodicCondition(IndexType NewId, GeometryType::Pointer pGeometry, PropertiesType::Pointer pProperties):
    Condition(NewId,pGeometry,pProperties)
{
}

template< unsigned int TDim >
TwoStepPeriodicCondition<TDim>::TwoStepPeriodicCondition(TwoStepPeriodicCondition const& rOther):
    Condition(rOther)
{
}

template< unsigned int TDim >
TwoStepPeriodicCondition<TDim>::~TwoStepPeriodicCondition()
{
}

template< unsigned int TDim >
TwoStepPeriodicCondition<TDim>& TwoStepPeriodicCondition<TDim>::operator =(TwoStepPeriodicCondition<TDim> const& rOther)
{
    Condition::operator =(rOther);

    return *this;
}

template< unsigned int TDim >
Condition::Pointer TwoStepPeriodicCondition<TDim>::Create(IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties) const
{
    return Condition::Pointer(new TwoStepPeriodicCondition(NewId, GetGeometry().Create(ThisNodes), pProperties));
}

template< unsigned int TDim >
int TwoStepPeriodicCondition<TDim>::Check(const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY;

    return Condition::Check(rCurrentProcessInfo);

    KRATOS_CATCH("");
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::CalculateLocalSystem(MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo)
{
    rLeftHandSideMatrix.resize(0,0,false);
    rRightHandSideVector.resize(0,false);
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::CalculateLeftHandSide(MatrixType& rLeftHandSideMatrix, ProcessInfo& rCurrentProcessInfo)
{
    rLeftHandSideMatrix.resize(0,0,false);
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::CalculateRightHandSide(VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo)
{
    MatrixType LHS;
    CalculateLocalSystem(LHS,rRightHandSideVector,rCurrentProcessInfo);
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::EquationIdVector(EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo)
{
    rResult.resize(0);
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::GetDofList(DofsVectorType& rElementalDofList, ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY;

    switch ( rCurrentProcessInfo[FRACTIONAL_STEP] )
    {
    case 1:
    {
        this->GetVelocityDofList(rElementalDofList,rCurrentProcessInfo);
        break;
    }
    case 5:
    {
        this->GetPressureDofList(rElementalDofList,rCurrentProcessInfo);
        break;
    }
    case 6:
    {
        this->GetVelocityDofList(rElementalDofList,rCurrentProcessInfo);
        break;
    }
    default:
    {
        KRATOS_THROW_ERROR(std::logic_error,"Unexpected value for FRACTIONAL_STEP index: ",rCurrentProcessInfo[FRACTIONAL_STEP]);
    }
    }

    KRATOS_CATCH("");
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::GetValuesVector(Vector& Values, int Step)
{
    if (Values.size() != 0)
        Values.resize(0,false);

/*    PeriodicVariablesContainer const& rPeriodicVariables = this->GetProperties().GetValue(PERIODIC_VARIABLES);
    const unsigned int BlockSize = rPeriodicVariables.size();
    const unsigned int LocalSize = 2 * BlockSize; // Total contribution size = 2 nodes * num dofs

    if (Values.size() != LocalSize)
        Values.resize(LocalSize,false);

    unsigned int LocalIndex = 0;

    for(PeriodicVariablesContainer::DoubleVariablesConstIterator itDVar = rPeriodicVariables.DoubleVariablesBegin();
            itDVar != rPeriodicVariables.DoubleVariablesEnd(); ++itDVar)
    {
        Values[LocalIndex] = this->GetGeometry()[0].FastGetSolutionStepValue(*itDVar,Step);
        Values[LocalIndex+BlockSize] = this->GetGeometry()[1].FastGetSolutionStepValue(*itDVar,Step);
        ++LocalIndex;
    }

    for(PeriodicVariablesContainer::VariableComponentsConstIterator itCVar = rPeriodicVariables.VariableComponentsBegin();
            itCVar != rPeriodicVariables.VariableComponentsEnd(); ++itCVar)
    {
        Values[LocalIndex] = this->GetGeometry()[0].FastGetSolutionStepValue(*itCVar,Step);
        Values[LocalIndex+BlockSize] = this->GetGeometry()[1].FastGetSolutionStepValue(*itCVar,Step);
        ++LocalIndex;
    }*/
}

template<>
void TwoStepPeriodicCondition<2>::GetVelocityDofList(DofsVectorType& rElementalDofList,
                                           ProcessInfo& rCurrentProcessInfo)
{
    GeometryType& rGeom = this->GetGeometry();
    const SizeType NumNodes = rGeom.PointsNumber();
    const SizeType LocalSize = 2*NumNodes;

    if (rElementalDofList.size() != LocalSize)
        rElementalDofList.resize(LocalSize);

    SizeType LocalIndex = 0;

    for (SizeType i = 0; i < NumNodes; ++i)
    {
        rElementalDofList[LocalIndex++] = rGeom[i].pGetDof(VELOCITY_X);
        rElementalDofList[LocalIndex++] = rGeom[i].pGetDof(VELOCITY_Y);
    }
}

template<>
void TwoStepPeriodicCondition<3>::GetVelocityDofList(DofsVectorType& rElementalDofList,
                                           ProcessInfo& rCurrentProcessInfo)
{
    GeometryType& rGeom = this->GetGeometry();
    const SizeType NumNodes = rGeom.PointsNumber();
    const SizeType LocalSize = 3*NumNodes;

    if (rElementalDofList.size() != LocalSize)
        rElementalDofList.resize(LocalSize);

    SizeType LocalIndex = 0;

    for (SizeType i = 0; i < NumNodes; ++i)
    {
        rElementalDofList[LocalIndex++] = rGeom[i].pGetDof(VELOCITY_X);
        rElementalDofList[LocalIndex++] = rGeom[i].pGetDof(VELOCITY_Y);
        rElementalDofList[LocalIndex++] = rGeom[i].pGetDof(VELOCITY_Z);
    }
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::GetPressureDofList(DofsVectorType& rElementalDofList,
                                              ProcessInfo& rCurrentProcessInfo)
{
    GeometryType& rGeom = this->GetGeometry();
    const SizeType NumNodes = rGeom.PointsNumber();

    if (rElementalDofList.size() != NumNodes)
        rElementalDofList.resize(NumNodes);

    SizeType LocalIndex = 0;

    for (SizeType i = 0; i < NumNodes; ++i)
    {
        rElementalDofList[LocalIndex++] = rGeom[i].pGetDof(PRESSURE);
    }
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::save(Serializer& rSerializer) const
{
    KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Condition );
}

template< unsigned int TDim >
void TwoStepPeriodicCondition<TDim>::load(Serializer& rSerializer)
{
    KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Condition );
}

/*
 * Template class definition 
 */

template class TwoStepPeriodicCondition<2>;
template class TwoStepPeriodicCondition<3>;

}
