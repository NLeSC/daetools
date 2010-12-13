#ifndef IDA_DAESOLVER_H
#define IDA_DAESOLVER_H

#include "../Core/helpers.h"
#include "solver_class_factory.h"
#include "dae_array_matrix.h"
#include "ida_la_solver_interface.h"
#include "../config.h"

namespace dae
{
namespace solver
{
/*********************************************************************************************
	daeIDASolver
**********************************************************************************************/
class daeIDASolverData;
class DAE_SOLVER_API daeIDASolver : public daeDAESolver_t
{
public:
	daeIDASolver(void);
	virtual ~daeIDASolver(void);

public:
	virtual void						Initialize(daeBlock_t* pBlock, 
												   daeLog_t* pLog, 
												   daeeInitialConditionMode eMode);
	virtual real_t						Solve(real_t dTime, daeeStopCriterion eCriterion);
	virtual void						SetRelativeTolerance(real_t relTol);
	virtual real_t						GetRelativeTolerance(void) const;
	virtual daeeInitialConditionMode	GetInitialConditionMode(void) const;
	virtual void						SetInitialConditionMode(daeeInitialConditionMode eMode);
	virtual daeBlock_t*					GetBlock(void) const;
	virtual daeLog_t*					GetLog(void) const;
	virtual void						RefreshRootFunctions(void);
	virtual void						Reinitialize(bool bCopyDataFromBlock);

	void SetLASolver(daeIDALASolver_t* pLASolver);

protected:
	virtual void CreateArrays(void);
	virtual void Set_InitialConditions_InitialGuesses_AbsRelTolerances(void);
	virtual void CreateIDA(void);
	virtual void CreateLinearSolver(void);
	virtual void SolveInitial(void);

	bool CheckFlag(int flag);
	string CreateIDAErrorMessage(int flag);
	
public:
//	void GetSparseMatrixData(int& nnz, int** ia, int** ja);
	void SaveMatrixAsXPM(const std::string& strFilename);
	void SaveMatrixAsPBM(const std::string& strFilename);

public:
	daeeInitialConditionMode			m_eInitialConditionMode;
	void*								m_pIDA;
	daeLog_t*							m_pLog;
	daeBlock_t*							m_pBlock;
	daeeIDALASolverType					m_eLASolver;
	daeIDALASolver_t*					m_pLASolver;
	size_t								m_nNumberOfEquations;
	real_t								m_dRelTolerance;
	real_t								m_timeStart;
	real_t								m_dCurrentTime;
	real_t								m_dTargetTime;
	real_t								m_dNextTimeAfterReinitialization;
	daeDenseArray						m_arrValues;
	daeDenseArray						m_arrTimeDerivatives;
	daeDenseArray						m_arrResiduals;
	daeDenseMatrix						m_matJacobian;
	boost::shared_ptr<daeIDASolverData>	m_pIDASolverData;
};


}
}

#endif
