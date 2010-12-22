#include "stdafx.h"
#include "coreimpl.h"

namespace dae 
{
namespace core 
{
/******************************************************************
	daeObjectiveFunction
*******************************************************************/
daeObjectiveFunction::daeObjectiveFunction(daeModel* pModel, real_t abstol)
{
	if(!pModel)
		daeDeclareAndThrowException(exInvalidPointer)
		
	const daeVariableType typeObjectiveFunction("typeObjectiveFunction", "-", -1.0e+100, 1.0e+100, 0.0, abstol);
	m_pModel			 = pModel;
	m_pObjectiveVariable = boost::shared_ptr<daeVariable>(new daeVariable("V_obj", typeObjectiveFunction, pModel, "Objective value"));
	m_pObjectiveVariable->SetReportingOn(true);
	m_pObjectiveFunction = pModel->CreateEquation("F_obj", "Objective function");
}

daeObjectiveFunction::~daeObjectiveFunction(void)
{
}

void daeObjectiveFunction::SetResidual(adouble res)
{
	if(!m_pObjectiveFunction)
		daeDeclareAndThrowException(exInvalidPointer)
	m_pObjectiveFunction->SetResidual( (*m_pObjectiveVariable )() - res);
}

adouble daeObjectiveFunction::GetResidual(void) const
{
	if(!m_pObjectiveFunction)
		daeDeclareAndThrowException(exInvalidPointer)
	return m_pObjectiveFunction->GetResidual();
}

void daeObjectiveFunction::Open(io::xmlTag_t* pTag)
{
	daeObject::Open(pTag);
}

void daeObjectiveFunction::Save(io::xmlTag_t* pTag) const
{
	daeObject::Save(pTag);
}

void daeObjectiveFunction::OpenRuntime(io::xmlTag_t* pTag)
{
	daeObject::OpenRuntime(pTag);
}

void daeObjectiveFunction::SaveRuntime(io::xmlTag_t* pTag) const
{
	daeObject::SaveRuntime(pTag);
}

bool daeObjectiveFunction::CheckObject(vector<string>& strarrErrors) const
{
	string strError;

	bool bCheck = true;

// Do not check daeObject since this is not an usual object
//	if(!daeObject::CheckObject(strarrErrors))
//		bCheck = false;

	if(!m_pObjectiveVariable)
	{	
		strError = "Invalid objective variable";
		strarrErrors.push_back(strError);
		return false;
	}
	if(!m_pObjectiveFunction)
	{	
		strError = "Invalid objective function";
		strarrErrors.push_back(strError);
		return false;
	}
	if(m_pObjectiveFunction->GetEquationDefinitionMode() == eEDMUnknown ||
	   m_pObjectiveFunction->GetEquationEvaluationMode() == eEEMUnknown)
	{	
		strError = "Constraint residual not specified [" + m_pObjectiveFunction->GetCanonicalName() + "]";
		strarrErrors.push_back(strError);
		return false;
	}
	
	return bCheck;
}


/******************************************************************
	daeOptimizationConstraint
*******************************************************************/
daeOptimizationConstraint::daeOptimizationConstraint(daeModel* pModel, real_t LB, real_t UB, real_t abstol, size_t N, string strDescription)
{
	if(!pModel)
		daeDeclareAndThrowException(exInvalidPointer)
		
	const daeVariableType typeConstraint("typeConstraint", "-", -1.0e+100, 1.0e+100, 0.0, abstol);
	string strVName = string("V_constraint") + toString<size_t>(N + 1); 
	string strFName = string("F_constraint") + toString<size_t>(N + 1); 

	m_pModel				= pModel;
	m_eConstraintType		= eInequalityConstraint;
	m_dLB					= LB;
	m_dUB					= UB;
	m_dValue				= 0;
	m_pConstraintVariable	= boost::shared_ptr<daeVariable>(new daeVariable(strVName, typeConstraint, m_pModel, strDescription));
	m_pConstraintVariable->SetReportingOn(true);
	m_pConstraintFunction	= m_pModel->CreateEquation(strFName, strDescription);
}

daeOptimizationConstraint::daeOptimizationConstraint(daeModel* pModel, real_t Value, real_t abstol, size_t N, string strDescription)
{
	if(!pModel)
		daeDeclareAndThrowException(exInvalidPointer)
		
	const daeVariableType typeConstraint("typeConstraint", "-", -1.0e+100, 1.0e+100, 0.0, abstol);
	string strVName = string("V_constraint") + toString<size_t>(N + 1); 
	string strFName = string("F_constraint") + toString<size_t>(N + 1); 

	m_pModel				= pModel;
	m_eConstraintType		= eEqualityConstraint;
	m_dLB					= 0;
	m_dUB					= 0;
	m_dValue				= Value;
	m_pConstraintVariable	= boost::shared_ptr<daeVariable>(new daeVariable(strVName, typeConstraint, m_pModel, strDescription));
	m_pConstraintFunction	= m_pModel->CreateEquation(strFName, strDescription);
}

daeOptimizationConstraint::~daeOptimizationConstraint(void)
{
}

void daeOptimizationConstraint::SetResidual(adouble res)
{
	if(!m_pConstraintFunction)
		daeDeclareAndThrowException(exInvalidPointer)
	m_pConstraintFunction->SetResidual( (*m_pConstraintVariable )() - res);
}

adouble daeOptimizationConstraint::GetResidual(void) const
{
	if(!m_pConstraintFunction)
		daeDeclareAndThrowException(exInvalidPointer)
	return m_pConstraintFunction->GetResidual();
}

void daeOptimizationConstraint::Open(io::xmlTag_t* pTag)
{
	daeObject::Open(pTag);
}

void daeOptimizationConstraint::Save(io::xmlTag_t* pTag) const
{
	daeObject::Save(pTag);
}

void daeOptimizationConstraint::OpenRuntime(io::xmlTag_t* pTag)
{
	daeObject::OpenRuntime(pTag);
}

void daeOptimizationConstraint::SaveRuntime(io::xmlTag_t* pTag) const
{
	daeObject::SaveRuntime(pTag);
}

bool daeOptimizationConstraint::CheckObject(vector<string>& strarrErrors) const
{
	string strError;

	bool bCheck = true;

// Do not check daeObject since this is not an usual object
//	if(!daeObject::CheckObject(strarrErrors))
//		bCheck = false;

	if(!m_pConstraintVariable)
	{	
		strError = "Invalid constraint variable";
		strarrErrors.push_back(strError);
		return false;
	}
	if(!m_pConstraintFunction)
	{	
		strError = "Invalid constraint function";
		strarrErrors.push_back(strError);
		return false;
	}
	if(m_pConstraintFunction->GetEquationDefinitionMode() == eEDMUnknown ||
	   m_pConstraintFunction->GetEquationEvaluationMode() == eEEMUnknown)
	{	
		strError = "Constraint residual not specified [" + m_pConstraintFunction->GetCanonicalName() + "]";
		strarrErrors.push_back(strError);
		return false;
	}
	
	return bCheck;
}

/******************************************************************
	daeOptimizationVariable
*******************************************************************/
daeOptimizationVariable::daeOptimizationVariable(const daeVariable* pVariable, real_t LB, real_t UB) : m_pVariable(pVariable)
{
	if(!m_pVariable)
		daeDeclareAndThrowException(exInvalidPointer)
			
	m_dLB = LB;
	m_dUB = UB;
}

daeOptimizationVariable::~daeOptimizationVariable(void)
{
}

size_t daeOptimizationVariable::GetIndex(void) const
{
	if(!m_pVariable)
		daeDeclareAndThrowException(exInvalidPointer)
		
	return m_pVariable->m_nOverallIndex;	
}

bool daeOptimizationVariable::CheckObject(vector<string>& strarrErrors) const
{
	string strError;

	bool bCheck = true;

// Do not check daeObject since this is not an usual object
//	if(!daeObject::CheckObject(strarrErrors))
//		bCheck = false;

	if(!m_pVariable)
	{	
		strError = "Invalid optimization variable specified";
		strarrErrors.push_back(strError);
		return false;
	}
	if(!m_pVariable->m_pModel || !m_pVariable->m_pModel->m_pDataProxy)
	{	
		strError = "Invalid parent model in optimization variable [" + m_pVariable->GetCanonicalName() + "]";
		strarrErrors.push_back(strError);
		return false;
	}
			
	if(!m_pVariable->m_ptrDomains.empty())
	{
		strError = "Optimization variable [" + m_pVariable->GetCanonicalName() + "] cannot be distributed";
		strarrErrors.push_back(strError);
		bCheck = false;
	}
  
	int type = m_pVariable->m_pModel->m_pDataProxy->GetVariableType(m_pVariable->m_nOverallIndex);
	if(type != cnFixed)
	{
		strError = "Optimization variable [" + m_pVariable->GetCanonicalName() + "] must be assigned (cannot be a state-variable)";
		strarrErrors.push_back(strError);
		bCheck = false;
	}

	return bCheck;
}




}
}
