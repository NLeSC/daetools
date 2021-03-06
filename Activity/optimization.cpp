#include "stdafx.h"
#include "simulation.h"
#include <stdio.h>
#include <time.h>

namespace daetools
{
namespace activity
{
daeOptimization::daeOptimization(void)
{
	m_pSimulation		 = NULL;
	m_pNLPSolver	     = NULL;
	m_pDAESolver		 = NULL;
	m_pDataReporter		 = NULL;
	m_pLog			     = NULL;
	m_bIsInitialized     = false;
	m_Initialization     = 0;
	m_Optimization       = 0;

	daeConfig& cfg = daeConfig::GetConfig();
}

daeOptimization::~daeOptimization(void)
{
}

void daeOptimization::Initialize(daeSimulation_t*   pSimulation,
								 daeNLPSolver_t*    pNLPSolver, 
								 daeDAESolver_t*    pDAESolver, 
								 daeDataReporter_t* pDataReporter, 
                                 daeLog_t*          pLog,
                                 const std::string& initializationFile)
{
	if(m_bIsInitialized)
	{
		daeDeclareException(exInvalidCall);
		e << "Optimization has already been initialized";
		throw e;
	}

	if(!pSimulation)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!pNLPSolver)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!pDAESolver)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!pDataReporter)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!pLog)
		daeDeclareAndThrowException(exInvalidPointer);

// Check data reporter
	if(!pDataReporter->IsConnected())
	{
		daeDeclareException(exInvalidCall);
		e << "Data Reporter is not connected \n";
		throw e;
	}

	m_Initialization = 0;
	m_Optimization   = 0;
	double start = daetools::GetTimeInSeconds();

	m_pSimulation		 = pSimulation;
	m_pNLPSolver	     = pNLPSolver;
	m_pDAESolver		 = pDAESolver;
	m_pDataReporter		 = pDataReporter;
	m_pLog			     = pLog;	
    m_InitializationFile = initializationFile;

	m_pSimulation->SetSimulationMode(eOptimization);
	m_pSimulation->Initialize(m_pDAESolver, m_pDataReporter, m_pLog, true);
    m_pNLPSolver->Initialize(this, m_pSimulation, m_pDAESolver, m_pDataReporter, m_pLog, m_InitializationFile);

	m_bIsInitialized = true;
	
	double end = daetools::GetTimeInSeconds();
	m_Initialization = end - start;
}

void daeOptimization::Run(void)
{
	if(!m_bIsInitialized)
	{
		daeDeclareException(exInvalidCall);
		e << "Optimization has not been initialized";
		throw e;
	}
	
	m_Optimization = daetools::GetTimeInSeconds();
	
	if(!m_pSimulation)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!m_pNLPSolver)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!m_pDAESolver)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!m_pDataReporter)
		daeDeclareAndThrowException(exInvalidPointer);
	if(!m_pLog)
		daeDeclareAndThrowException(exInvalidPointer);
	
	m_pNLPSolver->Solve();
	
	m_Optimization = daetools::GetTimeInSeconds() - m_Optimization;

	m_pLog->Message(string(" "), 0);
	m_pLog->Message(string("The optimization finished."), 0);
	m_pLog->Message(string("Total run time = ") + toStringFormatted<real_t>(double(m_Initialization + m_Optimization), -1, 3) + string(" s"), 0);
}

void daeOptimization::Finalize(void)
{
	if(m_pSimulation)
		m_pSimulation->Finalize();
	
	m_pSimulation	= NULL;
	m_pLog			= NULL;
	m_pNLPSolver	= NULL;
	m_pDataReporter = NULL;
	m_pDAESolver	= NULL;
	
	m_bIsInitialized = false;
}

void daeOptimization::StartIterationRun(int iteration)
{
    
}

void daeOptimization::EndIterationRun(int iteration)
{
    
}

}
}

