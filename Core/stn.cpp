#include "stdafx.h"
#include "coreimpl.h"

namespace daetools
{
namespace core
{
/*********************************************************************************************
    daeSTN
**********************************************************************************************/
daeSTN::daeSTN()
{
    m_pModel		= NULL;
    m_pParentState	= NULL;
    m_pActiveState	= NULL;
    m_eSTNType		= eSTN;
    m_bInitialized  = false;
}

daeSTN::~daeSTN()
{
}

void daeSTN::Clone(const daeSTN& rObject)
{
    for(size_t i = 0; i < rObject.m_ptrarrStates.size(); i++)
    {
        daeState* pState = AddState(rObject.m_ptrarrStates[i]->m_strShortName);
        pState->Clone(*rObject.m_ptrarrStates[i]);
    }
}

void daeSTN::UpdateEquations(void)
{
    if(!m_pActiveState)
        daeDeclareAndThrowException(exInvalidPointer);

    m_pActiveState->UpdateEquations();
}

void daeSTN::CleanUpSetupData()
{
    for(size_t i = 0; i < m_ptrarrStates.size(); i++)
        m_ptrarrStates[i]->CleanUpSetupData();
}

void daeSTN::Open(io::xmlTag_t* pTag)
{
    string strName;

    if(!m_pModel)
        daeDeclareAndThrowException(exInvalidPointer);

    m_ptrarrStates.EmptyAndFreeMemory();
    m_ptrarrStates.SetOwnershipOnPointers(true);

    daeObject::Open(pTag);

    strName = "Type";
    OpenEnum(pTag, strName, m_eSTNType);

    strName = "States";
    pTag->OpenObjectArray<daeState, daeState>(strName, m_ptrarrStates);

    strName = "ParentState";
    daeFindStateByID del(m_pModel);
    m_pParentState = pTag->OpenObjectRef(strName, &del);

    //ReconnectStateTransitionsAndStates();
}

void daeSTN::Save(io::xmlTag_t* pTag) const
{
    string strName;

    daeObject::Save(pTag);

    strName = "Type";
    SaveEnum(pTag, strName, m_eSTNType);

    strName = "States";
    pTag->SaveObjectArray(strName, m_ptrarrStates);

    strName = "ParentState";
    pTag->SaveObjectRef(strName, m_pParentState);
}

void daeSTN::Export(std::string& strContent, daeeModelLanguage eLanguage, daeModelExportContext& c) const
{
    string strExport, strStates;
    boost::format fmtFile;

    if(c.m_bExportDefinition)
    {
    }
    else
    {
        if(eLanguage == ePYDAE)
        {
            strExport = c.CalculateIndent(c.m_nPythonIndentLevel) + "self.%1% = self.STN(\"%2%\")\n\n" +
                        "%3%"+
                        c.CalculateIndent(c.m_nPythonIndentLevel) + "self.END_STN()\n\n";
            ExportObjectArray(m_ptrarrStates, strStates, eLanguage, c);

            fmtFile.parse(strExport);
            fmtFile % GetStrippedName()
                    % m_strShortName
                    % strStates;
            strContent += fmtFile.str();
        }
        else if(eLanguage == eCDAE)
        {
            strExport = c.CalculateIndent(c.m_nPythonIndentLevel) + "daeSTN* %1% = STN(\"%2%\");\n\n" +
                        "%3%"+
                        c.CalculateIndent(c.m_nPythonIndentLevel) + "END_STN();\n\n";
            ExportObjectArray(m_ptrarrStates, strStates, eLanguage, c);

            fmtFile.parse(strExport);
            fmtFile % GetStrippedName()
                    % m_strShortName
                    % strStates;
            strContent += fmtFile.str();
        }
        else
        {
            daeDeclareAndThrowException(exNotImplemented);
        }
    }
}

void daeSTN::OpenRuntime(io::xmlTag_t* pTag)
{
    daeObject::OpenRuntime(pTag);
}

void daeSTN::SaveRuntime(io::xmlTag_t* pTag) const
{
    string strName;

    daeObject::SaveRuntime(pTag);

    strName = "Type";
    SaveEnum(pTag, strName, m_eSTNType);

    strName = "States";
    pTag->SaveRuntimeObjectArray(strName, m_ptrarrStates);
}

string daeSTN::GetCanonicalName(void) const
{
    if(m_pParentState)
        return m_pParentState->GetCanonicalName() + '.' + m_strShortName;
    else
        return daeObject::GetCanonicalName();
}

void daeSTN::ReconnectStateTransitionsAndStates()
{
    daeDeclareAndThrowException(exInvalidCall);

//	size_t i, k;
//	daeState* pState;
//	daeOnConditionActions *pST;
//
//	for(i = 0; i < m_ptrarrStates.size(); i++)
//	{
//		pState = m_ptrarrStates[i];
//		if(!pState)
//			daeDeclareAndThrowException(exInvalidPointer);
//
//		for(k = 0; k < pState->m_ptrarrOnConditionActions.size(); k++)
//		{
//			pST = pState->m_ptrarrStateTransitions[k];
//			if(!pST)
//				daeDeclareAndThrowException(exInvalidPointer);
//
//			pST->m_pStateFrom = FindState(pST->m_nStateFromID);
//			if(!pST->m_pStateFrom)
//			{
//				daeDeclareException(exInvalidCall);
//				e << "Illegal start state in STN [" << GetCanonicalName() << "]";
//				throw e;
//			}
//
//			pST->m_pStateTo = FindState(pST->m_nStateToID);
//			if(!pST->m_pStateTo)
//			{
//				daeDeclareException(exInvalidCall);
//				e << "Illegal end state in STN [" << GetCanonicalName() << "]";
//				throw e;
//			}
//		}
//	}
}

daeState* daeSTN::FindState(long nID)
{
    size_t i;
    daeState* pState;

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        if(pState && pState->m_nID == (size_t)nID)
            return pState;
    }
    return NULL;
}

daeState* daeSTN::FindState(const string& strName)
{
    size_t i;
    daeState* pState;

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        if(pState && pState->GetName() == strName)
            return pState;
    }
    return NULL;
}

void daeSTN::FinalizeDeclaration()
{
    daeState *pState;

// Set the active state (default is the first)
    if(!GetActiveState())
    {
        if(m_ptrarrStates.size() > 0)
        {
            pState = m_ptrarrStates[0];
            SetActiveState(pState);
        }
    }

    m_bInitialized = true;
}

void daeSTN::InitializeOnEventAndOnConditionActions(void)
{
    size_t i;
    daeState *pState;

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        if(!pState)
            daeDeclareAndThrowException(exInvalidPointer);

        pState->InitializeOnEventAndOnConditionActions();
    }
}

void daeSTN::InitializeDEDIs(void)
{
    size_t i;
    daeState *pState;

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        if(!pState)
            daeDeclareAndThrowException(exInvalidPointer);

        pState->InitializeDEDIs();
    }
}

void daeSTN::CreateEquationExecutionInfo(void)
{
    size_t i, k, m;
    daeSTN* pSTN;
    daeState* pState;
    daeEquation* pEquation;
    vector<daeEquationExecutionInfo*> ptrarrEqnExecutionInfosCreated;

    if(!m_pModel)
        daeDeclareAndThrowException(exInvalidPointer);
    if(m_ptrarrStates.size() == 0)
    {
        daeDeclareException(exInvalidCall);
        e << "Number of states is 0; there must be at least two states in STN [" << GetCanonicalName() << "]";
        throw e;
    }

    for(k = 0; k < m_ptrarrStates.size(); k++)
    {
        pState = m_ptrarrStates[k];
        if(!pState)
            daeDeclareAndThrowException(exInvalidPointer);

        for(m = 0; m < pState->m_ptrarrEquations.size(); m++)
        {
            pEquation = pState->m_ptrarrEquations[m];
            if(!pEquation)
                daeDeclareAndThrowException(exInvalidPointer);

        // Create EqnExecInfos, call GatherInfo for each of them, but DON'T add them to the model
        // They are added to the vector which belongs to the state
            ptrarrEqnExecutionInfosCreated.clear();
            pEquation->CreateEquationExecutionInfos(m_pModel, ptrarrEqnExecutionInfosCreated, false);

        // Now add all of them to the state
            pState->m_ptrarrEquationExecutionInfos.insert(pState->m_ptrarrEquationExecutionInfos.end(),
                                                          ptrarrEqnExecutionInfosCreated.begin(),
                                                          ptrarrEqnExecutionInfosCreated.end());
        }

        for(i = 0; i < pState->m_ptrarrSTNs.size(); i++)
        {
            pSTN = pState->m_ptrarrSTNs[i];
            if(!pSTN)
                daeDeclareAndThrowException(exInvalidPointer);

            pSTN->CreateEquationExecutionInfo();
        }
    }

}

void daeSTN::CollectEquationExecutionInfos(vector<daeEquationExecutionInfo*>& ptrarrEquationExecutionInfo)
{
    size_t i, k;
    daeSTN* pSTN;

    if(!m_pActiveState)
        daeDeclareAndThrowException(exInvalidPointer);

    dae_add_vector(m_pActiveState->m_ptrarrEquationExecutionInfos, ptrarrEquationExecutionInfo);

// Nested STNs
    for(i = 0; i < m_pActiveState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = m_pActiveState->m_ptrarrSTNs[i];
        if(!pSTN)
            daeDeclareAndThrowException(exInvalidPointer);

        pSTN->CollectEquationExecutionInfos(ptrarrEquationExecutionInfo);
    }
}

void daeSTN::AddEquationsOverallIndexes(map<size_t, vector<size_t> >& mapIndexes)
{

}

void daeSTN::CollectVariableIndexes(map<size_t, size_t>& mapVariableIndexes)
{
    size_t i, k, m;
    daeSTN* pSTN;
    daeState* pState;
    daeOnConditionActions* pOnConditionAction;
    daeEquationExecutionInfo* pEquationExecutionInfo;
    pair<size_t, size_t> uintPair;
    map<size_t, size_t>::iterator iter;

// Collect indexes from the states
    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        if(!pState)
            daeDeclareAndThrowException(exInvalidPointer);

        for(k = 0; k < pState->m_ptrarrEquationExecutionInfos.size(); k++)
        {
            pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[k];
            if(!pEquationExecutionInfo)
                daeDeclareAndThrowException(exInvalidPointer);

            for(iter = pEquationExecutionInfo->m_mapIndexes.begin(); iter != pEquationExecutionInfo->m_mapIndexes.end(); iter++)
            {
                uintPair.first  = (*iter).first;
                uintPair.second = mapVariableIndexes.size(); // doesn't matter what value - it is not used anywhere
                mapVariableIndexes.insert(uintPair);
            }
        }

    // Collect indexes from the nested STNs
        for(m = 0; m < pState->m_ptrarrSTNs.size(); m++)
        {
            pSTN = pState->m_ptrarrSTNs[m];
            if(!pSTN)
                daeDeclareAndThrowException(exInvalidPointer);

            pSTN->CollectVariableIndexes(mapVariableIndexes);
        }
    }

// Collect indexes from the conditions
    for(k = 0; k < m_ptrarrStates.size(); k++)
    {
        pState = m_ptrarrStates[k];
        if(!pState)
            daeDeclareAndThrowException(exInvalidPointer);

        for(m = 0; m < pState->m_ptrarrOnConditionActions.size(); m++)
        {
            pOnConditionAction = pState->m_ptrarrOnConditionActions[m];
            if(!pOnConditionAction)
                daeDeclareAndThrowException(exInvalidPointer);

            // Achtung, Achtung!!
            // Is this wise (or necessary)? Why do we add it to the list of indexes?
            // Double-check its ramifications!!!
            pOnConditionAction->m_Condition.m_pConditionNode->AddVariableIndexToArray(mapVariableIndexes, false);
        }
    }
}

void daeSTN::SetIndexesWithinBlockToEquationExecutionInfos(daeBlock* pBlock, size_t& nEquationIndex)
{
    size_t i, k, m, nTempEquationIndex;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;
    map<size_t, size_t>::iterator iter, iterIndexInBlock;

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        if(!pState)
            daeDeclareAndThrowException(exInvalidPointer);

        // Indexes are the same in each state;
        // State1: 15, 16, 17
        // State2: 15, 16, 17
        // etc ...
        nTempEquationIndex = nEquationIndex;
        for(k = 0; k < pState->m_ptrarrEquationExecutionInfos.size(); k++)
        {
            pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[k];
            if(!pEquationExecutionInfo)
                daeDeclareAndThrowException(exInvalidPointer);

            pEquationExecutionInfo->m_nEquationIndexInBlock = nTempEquationIndex;
            //pEquationExecutionInfo->m_pBlock = pBlock;
//------------------->
        // Here I have to associate overall variable indexes in equation to corresponding indexes in the block
        // m_mapIndexes<OverallIndex, BlockIndex>
            for(iter = pEquationExecutionInfo->m_mapIndexes.begin(); iter != pEquationExecutionInfo->m_mapIndexes.end(); iter++)
            {
            // Try to find OverallIndex in the map of BlockIndexes
                iterIndexInBlock = pBlock->m_mapVariableIndexes.find((*iter).first);
                if(iterIndexInBlock == pBlock->m_mapVariableIndexes.end())
                {
                    daeDeclareException(exInvalidCall);
                    e << "Cannot find overall variable index [" << toString<size_t>((*iter).first) << "] in stn " << GetCanonicalName();
                    throw e;
                }
                (*iter).second = (*iterIndexInBlock).second;
            }
//------------------->
            nTempEquationIndex++;
        }
    // Nested STNs
        for(m = 0; m < pState->m_ptrarrSTNs.size(); m++)
        {
            pSTN = pState->m_ptrarrSTNs[m];
            if(!pSTN)
                daeDeclareAndThrowException(exInvalidPointer);
        // Here I use nTempEquationIndex since I continue counting equations in the same state
            pSTN->SetIndexesWithinBlockToEquationExecutionInfos(pBlock, nTempEquationIndex);
        }
    }

    nEquationIndex = nTempEquationIndex;
}

uint32_t daeSTN::GetComputeStackSize()
{
    size_t i, k, m;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;

    uint32_t noItems = 0;
    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];

        for(k = 0; k < pState->m_ptrarrEquationExecutionInfos.size(); k++)
        {
            pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[k];
            noItems += adNode::GetComputeStackSize(pEquationExecutionInfo->m_EquationEvaluationNode.get());
        }

    // Nested STNs
        for(m = 0; m < pState->m_ptrarrSTNs.size(); m++)
        {
            pSTN = pState->m_ptrarrSTNs[m];
            noItems += pSTN->GetComputeStackSize();
        }
    }
    return noItems;
}

void daeSTN::CreateComputeStack(daeBlock* pBlock)
{
    size_t i, k, m;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];

        for(k = 0; k < pState->m_ptrarrEquationExecutionInfos.size(); k++)
        {
            pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[k];
            pEquationExecutionInfo->CreateComputeStack(pBlock);
        }

    // Nested STNs
        for(m = 0; m < pState->m_ptrarrSTNs.size(); m++)
        {
            pSTN = pState->m_ptrarrSTNs[m];
            pSTN->CreateComputeStack(pBlock);
        }
    }
}

void daeSTN::BuildJacobianExpressions()
{
    size_t i, k, m;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];

        for(k = 0; k < pState->m_ptrarrEquationExecutionInfos.size(); k++)
        {
            pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[k];

            if(pEquationExecutionInfo->m_pEquation->m_bBuildJacobianExpressions)
                pEquationExecutionInfo->BuildJacobianExpressions();
        }

    // Nested STNs
        for(m = 0; m < pState->m_ptrarrSTNs.size(); m++)
        {
            pSTN = pState->m_ptrarrSTNs[m];
            pSTN->BuildJacobianExpressions();
        }
    }
}

void daeSTN::AddExpressionsToBlock(daeBlock* pBlock)
{
    size_t i;
    daeSTN* pSTN;
    daeState* pState;
    daeOnConditionActions* pOnConditionActions;
    pair<size_t, daeExpressionInfo> pairExprInfo;
    map<size_t, daeExpressionInfo>::iterator iter;

    pState = m_pActiveState;
    if(!pState)
    {
        daeDeclareException(exInvalidCall);
        e << "Active state does not exist in STN [" << GetCanonicalName() << "]";
        throw e;
    }

    for(i = 0; i < pState->m_ptrarrOnConditionActions.size(); i++)
    {
        pOnConditionActions = pState->m_ptrarrOnConditionActions[i];
        if(!pOnConditionActions)
            daeDeclareAndThrowException(exInvalidPointer);

        for(iter = pOnConditionActions->m_mapExpressionInfos.begin(); iter != pOnConditionActions->m_mapExpressionInfos.end(); iter++)
        {
            pairExprInfo		= *iter;
            pairExprInfo.first	= pBlock->m_mapExpressionInfos.size();
            pBlock->m_mapExpressionInfos.insert(pairExprInfo);
        }
    }

// Nested STNs
    for(i = 0; i < pState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = pState->m_ptrarrSTNs[i];
        if(!pSTN)
            daeDeclareAndThrowException(exInvalidPointer);

        pSTN->AddExpressionsToBlock(pBlock);
    }
}

void daeSTN::BuildExpressions(daeBlock* pBlock)
{
    size_t i, k, m;
    daeSTN* pSTN;
    daeState* pState;
    daeOnConditionActions* pOnConditionActions;
    pair<size_t, daeExpressionInfo> pairExprInfo;

    if(!m_pModel)
        daeDeclareAndThrowException(exInvalidPointer);

    daeExecutionContext EC;
    EC.m_pDataProxy					= m_pModel->m_pDataProxy.get();
    EC.m_pBlock						= pBlock;
    EC.m_eEquationCalculationMode	= eCreateFunctionsIFsSTNs;

// I have to set this since Create_adouble called from adSetup nodes needs it
    std::shared_ptr<daeDataProxy_t> pDataProxy = m_pModel->GetDataProxy();
    daeModel* pTopLevelModel = dynamic_cast<daeModel*>(pDataProxy->GetTopLevelModel());
    pTopLevelModel->PropagateGlobalExecutionContext(&EC);

    for(i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        if(!pState)
            daeDeclareAndThrowException(exInvalidPointer);

        size_t nIndexInState = 0;
        for(k = 0; k < pState->m_ptrarrOnConditionActions.size(); k++)
        {
            pOnConditionActions = pState->m_ptrarrOnConditionActions[k];
            if(!pOnConditionActions)
                daeDeclareAndThrowException(exInvalidPointer);

        // Fill array with expressions of the form: left - right,
        // made of the conditional expressions, like: left >= right ... etc etc
            m_pModel->m_pDataProxy->SetGatherInfo(true);
            pBlock->SetInitializeMode(true);
                pOnConditionActions->m_Condition.BuildExpressionsArray(&EC);
                for(m = 0; m < pOnConditionActions->m_Condition.m_ptrarrExpressions.size(); m++)
                {
                    pairExprInfo.first                        = nIndexInState;
                    pairExprInfo.second.m_pExpression         = pOnConditionActions->m_Condition.m_ptrarrExpressions[m];
                    pairExprInfo.second.m_pOnConditionActions = pOnConditionActions;

                    pOnConditionActions->m_mapExpressionInfos.insert(pairExprInfo);
                    nIndexInState++;
                }
            m_pModel->m_pDataProxy->SetGatherInfo(false);
            pBlock->SetInitializeMode(false);
        }

    // Nested STNs
        for(m = 0; m < pState->m_ptrarrSTNs.size(); m++)
        {
            pSTN = pState->m_ptrarrSTNs[m];
            if(!pSTN)
                daeDeclareAndThrowException(exInvalidPointer);

            pSTN->BuildExpressions(pBlock);
        }
    }

// Restore it to NULL
    pTopLevelModel->PropagateGlobalExecutionContext(NULL);
}

// This function ONLY checks if there is a discontinuity somewhere; if there is - it returns true; otherwise false
// It will return true if any condition in any STN or nested STN has been satisfied
bool daeSTN::CheckDiscontinuities(void)
{
    size_t i;
    daeSTN* pSTN;
    daeOnConditionActions* pOnConditionActions;

    if(!m_pActiveState)
    {
        daeDeclareException(exInvalidCall);
        e << "Active state does not exist in STN [" << GetCanonicalName() << "]";
        throw e;
    }

    daeExecutionContext EC;
    EC.m_pDataProxy					= m_pModel->m_pDataProxy.get();
    EC.m_pBlock						= m_pModel->m_pDataProxy->GetBlock();
    EC.m_eEquationCalculationMode	= eCalculate;

    for(i = 0; i < m_pActiveState->m_ptrarrOnConditionActions.size(); i++)
    {
        pOnConditionActions = m_pActiveState->m_ptrarrOnConditionActions[i];
        if(pOnConditionActions->m_Condition.Evaluate(&EC)) // There is a discontinuity, therefore return true
        {
            m_pModel->m_pDataProxy->SetLastSatisfiedCondition(&pOnConditionActions->m_Condition);
            return true;
        }
    }

// Now I have to check for discontinuities in the nested STNs of the current active state
    for(i = 0; i < m_pActiveState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = m_pActiveState->m_ptrarrSTNs[i];
        if(pSTN->CheckDiscontinuities()) // There is a discontinuity, therefore return true
            return true;
    }

    return false;
}

void daeSTN::ExecuteOnConditionActions(void)
{
    size_t i;
    daeSTN* pSTN;
    daeOnConditionActions* pOnConditionActions;

    if(!m_pActiveState)
    {
        daeDeclareException(exInvalidCall);
        e << "Active state does not exist in STN [" << GetCanonicalName() << "]";
        throw e;
    }

    daeExecutionContext EC;
    EC.m_pDataProxy					= m_pModel->m_pDataProxy.get();
    EC.m_pBlock						= m_pModel->m_pDataProxy->GetBlock();
    EC.m_eEquationCalculationMode	= eCalculate;

    for(i = 0; i < m_pActiveState->m_ptrarrOnConditionActions.size(); i++)
    {
        pOnConditionActions = m_pActiveState->m_ptrarrOnConditionActions[i];

        if(pOnConditionActions->m_Condition.Evaluate(&EC))
        {
            if(m_pModel->m_pDataProxy->PrintInfo())
                LogMessage(string("The condition: ") + pOnConditionActions->GetConditionAsString() + string(" is satisfied"), 0);

            pOnConditionActions->Execute();
            break;
        }
    }

// Now I have to check state transitions in the nested STNs of the current active state
// m_pActiveState might point now to the new state (if the state-change occured in actions above)
    for(i = 0; i < m_pActiveState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = m_pActiveState->m_ptrarrSTNs[i];
        pSTN->ExecuteOnConditionActions();
    }
}

/* Old CheckDiscontinuities() function
bool daeSTN::CheckDiscontinuities(void)
{
    size_t i;
    daeSTN* pSTN;
    daeOnConditionActions* pOnConditionActions;

    if(!m_pActiveState)
    {
        daeDeclareException(exInvalidCall);
        e << "Active state does not exist in STN [" << GetCanonicalName() << "]";
        throw e;
    }

    daeExecutionContext EC;
    EC.m_pDataProxy					= m_pModel->m_pDataProxy.get();
  EC.m_pBlock						= m_pModel->m_pDataProxy->GetBlock();
    EC.m_eEquationCalculationMode	= eCalculate;

    // Save the current active state to check if it has been changed by one of the actions
    daeState* pFormerActiveState = m_pActiveState;

    bool bResult = false;
    for(i = 0; i < m_pActiveState->m_ptrarrOnConditionActions.size(); i++)
    {
        pOnConditionActions = m_pActiveState->m_ptrarrOnConditionActions[i];
        if(!pOnConditionActions)
            daeDeclareAndThrowException(exInvalidPointer);

        if(pOnConditionActions->m_Condition.Evaluate(&EC))
        {
            LogMessage(string("The condition: ") + pOnConditionActions->GetConditionAsString() + string(" is satisfied"), 0);

        // Execute the actions
            pOnConditionActions->Execute();

        // If the active state has changed then set the flag and break
            if(pFormerActiveState != m_pActiveState)
            {
                bResult = true;
                break;
            }
            //return CheckState(pOnConditionActions->m_pStateTo);
        }
    }

// Now I have to check state transitions in the nested STNs of the current active state
// m_pActiveState might point now to the new state (if the state-change occured in actions above)
    for(i = 0; i < m_pActiveState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = m_pActiveState->m_ptrarrSTNs[i];
        if(!pSTN)
            daeDeclareAndThrowException(exInvalidPointer);

        if(pSTN->CheckDiscontinuities())
            bResult = true;
    }
    return bResult;
}
*/

// Not used anymore
bool daeSTN::CheckState(daeState* pState)
{
/*
    daeSTN* pSTN;

    if(!pState)
        daeDeclareAndThrowException(exInvalidPointer);

// Only if the current state is not equal to the active state change the state (no reinitialization)
// but continue searching for state change in the nested IF/STNs
    bool bResult = false;
    if(m_pActiveState != pState)
    {
        bResult = true;
        SetActiveState(pState);
    }
    else
    {
        LogMessage(string("Current state unchanged"), 0);
    }

// Check nested STNs no matter if the active state has or has not been changed
    for(size_t i = 0; i < pState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = pState->m_ptrarrSTNs[i];
        if(!pSTN)
            daeDeclareAndThrowException(exInvalidPointer);

        if(pSTN->CheckDiscontinuities())
            bResult = true;
    }

    return bResult;
*/
    return true;
}

size_t daeSTN::GetNumberOfEquations() const
{
    daeState* pState;

    if(m_ptrarrStates.empty())
        daeDeclareException(exInvalidCall);

    pState = m_ptrarrStates[0];
    if(!pState)
        daeDeclareException(exInvalidPointer);

    return GetNumberOfEquationsInState(pState);
}

size_t daeSTN::GetNumberOfStates(void) const
{
    return m_ptrarrStates.size();
}

size_t daeSTN::GetNumberOfEquationsInState(daeState* pState) const
{
    size_t i, nNoEqns;
    daeSTN* pSTN;
    daeEquation* pEquation;

    if(!pState)
        daeDeclareAndThrowException(exInvalidPointer);

    nNoEqns = 0;
    for(i = 0; i < pState->m_ptrarrEquations.size(); i++)
    {
        pEquation = pState->m_ptrarrEquations[i];
        nNoEqns += pEquation->GetNumberOfEquations();
    }

    for(i = 0; i < pState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = pState->m_ptrarrSTNs[i];
        nNoEqns += pSTN->GetNumberOfEquations();
    }

    return nNoEqns;
}

daeState* daeSTN::GetParentState(void) const
{
    return m_pParentState;
}

const std::vector<daeState*>& daeSTN::States() const
{
    return m_ptrarrStates;
}

void daeSTN::CollectAllSTNs(std::map<std::string, daeSTN_t*>& mapSTNs) const
{
    daeSTN* pSTN;
    daeState* pState;

    for(size_t i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];

        for(size_t k = 0; k < pState->m_ptrarrSTNs.size(); k++)
        {
            pSTN = pState->m_ptrarrSTNs[k];

            if(pSTN->GetType() == eSTN)
                mapSTNs[pSTN->GetCanonicalName()] = pSTN;

            pSTN->CollectAllSTNs(mapSTNs);
        }
    }
}

void daeSTN::SetParentState(daeState* pParentState)
{
    if(!pParentState)
        daeDeclareAndThrowException(exInvalidPointer);
    m_pParentState = pParentState;
}

daeeSTNType daeSTN::GetType(void) const
{
    return m_eSTNType;
}

void daeSTN::SetType(daeeSTNType eType)
{
    m_eSTNType = eType;
}

daeState* daeSTN::AddState(const string& strName)
{
// Instantiate a new state and add it to this STN
    daeState* pState = new daeState();

    dae_push_back(m_ptrarrStates, pState);

    pState->Create(strName, this);

    return pState;
}

void daeSTN::GetStates(vector<daeState_t*>& ptrarrStates)
{
    ptrarrStates.clear();
    for(size_t i = 0; i < m_ptrarrStates.size(); i++)
        ptrarrStates.push_back(m_ptrarrStates[i]);
}

void daeSTN::SetActiveState(const string& strStateName)
{
    daeState* pState = FindState(strStateName);

    if(!pState)
    {
        daeDeclareException(exInvalidCall);
        e << "The state [" << strStateName << "] does not exist in STN [" << GetCanonicalName() << "]";
        throw e;
    }

    SetActiveState(pState);
}

string daeSTN::GetActiveState2(void) const
{
    if(!m_pActiveState)
        daeDeclareAndThrowException(exInvalidPointer);

    return m_pActiveState->GetName();
}

void daeSTN::SetActiveState(daeState* pState)
{
    if(!pState)
        daeDeclareAndThrowException(exInvalidPointer);

    // If it is the same state - do nothing
    if(m_pActiveState == pState)
        return;

    if(m_pModel->m_pDataProxy->PrintInfo())
        LogMessage(string("The state: ") + pState->GetCanonicalName() + string(" is active now"), 0);

    m_pModel->m_pDataProxy->SetReinitializationFlag(true);

// Disconnect old OnEventActions
    if(m_pActiveState)
        m_pActiveState->DisconnectOnEventActions();

// Connect new OnEventActions
    pState->ConnectOnEventActions();

// Set the new active state
    m_pActiveState = pState;
}

daeState_t* daeSTN::GetActiveState()
{
    return m_pActiveState;
}

void daeSTN::CalculateResiduals(daeExecutionContext& EC)
{
    size_t i;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;

    pState = m_pActiveState;
    if(!pState)
        daeDeclareAndThrowException(exInvalidPointer);

    for(i = 0; i < pState->m_ptrarrEquationExecutionInfos.size(); i++)
    {
        pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[i];
        pEquationExecutionInfo->Residual(EC);
    }
// Nested STNs
    for(i = 0; i < pState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = pState->m_ptrarrSTNs[i];
        pSTN->CalculateResiduals(EC);
    }
}

void daeSTN::CalculateJacobian(daeExecutionContext& EC)
{
    size_t i;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;

    pState = m_pActiveState;
    if(!pState)
        daeDeclareAndThrowException(exInvalidPointer);

    for(i = 0; i < pState->m_ptrarrEquationExecutionInfos.size(); i++)
    {
        pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[i];
        pEquationExecutionInfo->Jacobian(EC);
    }
// Nested STNs
    for(i = 0; i < pState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = pState->m_ptrarrSTNs[i];
        pSTN->CalculateJacobian(EC);
    }
}

void daeSTN::CalculateSensitivityResiduals(daeExecutionContext& EC, const std::vector<size_t>& narrParameterIndexes)
{
    size_t i;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;

    pState = m_pActiveState;
    if(!pState)
        daeDeclareAndThrowException(exInvalidPointer);

    for(i = 0; i < pState->m_ptrarrEquationExecutionInfos.size(); i++)
    {
        pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[i];
        pEquationExecutionInfo->SensitivityResiduals(EC, narrParameterIndexes);
    }
// Nested STNs
    for(i = 0; i < pState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = pState->m_ptrarrSTNs[i];
        pSTN->CalculateSensitivityResiduals(EC, narrParameterIndexes);
    }
}

void daeSTN::CalculateSensitivityParametersGradients(daeExecutionContext& EC, const std::vector<size_t>& narrParameterIndexes)
{
    size_t i;
    daeSTN* pSTN;
    daeState* pState;
    daeEquationExecutionInfo* pEquationExecutionInfo;

    pState = m_pActiveState;
    if(!pState)
        daeDeclareAndThrowException(exInvalidPointer);

    for(i = 0; i < pState->m_ptrarrEquationExecutionInfos.size(); i++)
    {
        pEquationExecutionInfo = pState->m_ptrarrEquationExecutionInfos[i];
        pEquationExecutionInfo->SensitivityParametersGradients(EC, narrParameterIndexes);
    }
// Nested STNs
    for(i = 0; i < pState->m_ptrarrSTNs.size(); i++)
    {
        pSTN = pState->m_ptrarrSTNs[i];
        pSTN->CalculateSensitivityParametersGradients(EC, narrParameterIndexes);
    }
}

void daeSTN::CalcNonZeroElements(int& NNZ)
{
    if(m_pModel->m_pDataProxy->ResetLAMatrixAfterDiscontinuity())
    {
        m_pActiveState->CalcNonZeroElements(NNZ);
    }
    else
    {
    // First add all indexes from equations to a map (one map per equation).
    // Then add the number of those indexes in each combined equation to the NNZ
        daeState* pState;
        size_t i, nCurrentEquaton, neq;
        vector< map<size_t, size_t> > arrIndexes;

        neq = GetNumberOfEquations();
        arrIndexes.resize(neq);

        for(i = 0; i < m_ptrarrStates.size(); i++)
        {
            pState = m_ptrarrStates[i];
            //if(pState->GetNumberOfEquations() != arrIndexes.size())
            //	daeDeclareAndThrowException(exInvalidCall);

        // Get the vector with the map<OverallIndex, BlockIndex> for each equation in the state
            nCurrentEquaton = 0;
            pState->AddIndexesFromAllEquations(arrIndexes, nCurrentEquaton);
        }

        for(i = 0; i < arrIndexes.size(); i++)
            NNZ += arrIndexes[i].size();
    }
}

void daeSTN::FillSparseMatrix(daeSparseMatrix<real_t>* pMatrix)
{
    if(m_pModel->m_pDataProxy->ResetLAMatrixAfterDiscontinuity())
    {
        m_pActiveState->FillSparseMatrix(pMatrix);
    }
    else
    {
        daeState* pState;
        size_t i, nCurrentEquaton, neq;
        vector< map<size_t, size_t> > arrIndexes;

        neq = GetNumberOfEquations();
        arrIndexes.resize(neq);

        for(i = 0; i < m_ptrarrStates.size(); i++)
        {
            pState = m_ptrarrStates[i];
            //if(pState->GetNumberOfEquations() != arrIndexes.size())
            //	daeDeclareAndThrowException(exInvalidCall);

        // Get the vector with the map<OverallIndex, BlockIndex> for each equation in the state
            nCurrentEquaton = 0;
            pState->AddIndexesFromAllEquations(arrIndexes, nCurrentEquaton);
        }

        for(i = 0; i < neq; i++)
            pMatrix->AddRow(arrIndexes[i]);

/*		std::cout << GetCanonicalName() << " arrIndexes:  " << std::endl;
        for(i = 0; i < neq; i++)
        {
            for(std::map<size_t, size_t>::iterator iter = arrIndexes[i].begin(); iter != arrIndexes[i].end(); iter++)
                std::cout << "(" + toString(iter->first) + ", " + toString(iter->second) + ") ";
            std::cout << std::endl;
        }
        std::cout << std::endl;*/
    }
}

void daeSTN::AddIndexesFromAllEquations(std::vector< std::map<size_t, size_t> >& arrIndexes, size_t& nCurrentEquaton)
{
    daeState* pState;

    for(size_t i = 0; i < m_ptrarrStates.size(); i++)
    {
        pState = m_ptrarrStates[i];
        pState->AddIndexesFromAllEquations(arrIndexes, nCurrentEquaton);
    }
}

bool daeSTN::CheckObject(vector<string>& strarrErrors) const
{
    string strError;
    size_t nNoEquationsInEachState = 0;

    bool bCheck = true;

    dae_capacity_check(m_ptrarrStates);

// Check base class
    if(!daeObject::CheckObject(strarrErrors))
        bCheck = false;

// If parent state is not null then it is nested stn
//	if(m_pParentState);

// Check the active state
    if(!m_pActiveState)
    {
        strError = "Invalid active state in state transition network [" + GetCanonicalName() + "]";
        strarrErrors.push_back(strError);
        bCheck = false;
    }

// Check the type
    if(m_eSTNType == eSTNTUnknown)
    {
        strError = "Invalid type in state transition network [" + GetCanonicalName() + "]";
        strarrErrors.push_back(strError);
        bCheck = false;
    }

// Check whether the stn is initialized
    if(!m_bInitialized)
    {
        strError = "Uninitialized state transition network [" + GetCanonicalName() + "]";
        strarrErrors.push_back(strError);
        bCheck = false;
    }

// Check number of states
    if(m_ptrarrStates.size() == 0)
    {
        strError = "Invalid number of states in state transition network [" + GetCanonicalName() + "]";
        strarrErrors.push_back(strError);
        bCheck = false;
    }

// Check states
    if(m_ptrarrStates.size() > 0)
    {
        daeState* pState;
        for(size_t i = 0; i < m_ptrarrStates.size(); i++)
        {
            pState = m_ptrarrStates[i];
            if(!pState)
            {
                strError = "Invalid state in state transition network [" + GetCanonicalName() + "]";
                strarrErrors.push_back(strError);
                bCheck = false;
                continue;
            }

        // Number of equations must be the same in all states
            if(i == 0)
            {
                nNoEquationsInEachState = GetNumberOfEquationsInState(pState);
            }
            else
            {
                if(nNoEquationsInEachState != GetNumberOfEquationsInState(pState))
                {
                    strError = "Number of equations must be the same in all states in state transition network [" + GetCanonicalName() + "]";
                    strarrErrors.push_back(strError);
                    bCheck = false;
                }
            }

            if(!pState->CheckObject(strarrErrors))
                bCheck = false;
        }
    }

    return bCheck;
}


}
}

