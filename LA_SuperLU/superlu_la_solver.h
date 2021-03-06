#ifndef SUPERLU_LA_SOLVER_H
#define SUPERLU_LA_SOLVER_H

#include "../IDAS_DAESolver/ida_la_solver_interface.h"
#include "../IDAS_DAESolver/solver_class_factory.h"
#include "../IDAS_DAESolver/dae_array_matrix.h"
#include "superlu_solver.h"
#include "superlu_mt_solver.h"

#ifdef daeSuperLU_MT
extern "C"
{
#include <slu_mt_ddefs.h>
}
#endif

#ifdef daeSuperLU
extern "C"
{
#include <slu_ddefs.h>
}
#endif


namespace daetools
{
namespace solver
{
#ifdef daeSuperLU
namespace superlu
{
#endif
#ifdef daeSuperLU_MT
namespace superlu_mt
{
#endif

class DAE_SUPERLU_API daeSuperLUSolver : public daetools::solver::daeLASolver_t
{
    typedef daeCSRMatrix<real_t, int> daeSuperLUMatrix;
public:
    daeSuperLUSolver();
    ~daeSuperLUSolver();

public:
    virtual int Create(size_t n,
                       size_t nnz,
                       daeBlockOfEquations_t* block);
    virtual int Reinitialize(size_t nnz);
    virtual int Init();
    virtual int Setup(real_t  time,
                      real_t  inverseTimeStep,
                      real_t* pdValues,
                      real_t* pdTimeDerivatives,
                      real_t* pdResiduals);
    virtual int Solve(real_t  time,
                      real_t  inverseTimeStep,
                      real_t  cjratio,
                      real_t* b,
                      real_t* weight,
                      real_t* pdValues,
                      real_t* pdTimeDerivatives,
                      real_t* pdResiduals);
    virtual int Free();
    virtual int SaveAsXPM(const std::string& strFileName);
    virtual int SaveAsMatrixMarketFile(const std::string& strFileName, const std::string& strMatrixName, const std::string& strMatrixDescription);

    virtual string GetName(void) const;

    virtual std::map<std::string, call_stats::TimeAndCount> GetCallStats() const;

    virtual void        SetOption_string(const std::string& strName, const std::string& Value);
    virtual void        SetOption_float(const std::string& strName, double Value);
    virtual void        SetOption_int(const std::string& strName, int Value);
    virtual void        SetOption_bool(const std::string& strName, bool Value);

    virtual std::string GetOption_string(const std::string& strName);
    virtual double      GetOption_float(const std::string& strName);
    virtual int         GetOption_int(const std::string& strName);
    virtual bool        GetOption_bool(const std::string& strName);

#ifdef daeSuperLU_MT
    superlumt_options_t& GetOptions(void);
#endif

#ifdef daeSuperLU
    superlu_options_t& GetOptions(void);
#endif

protected:
    void InitializeSuperLU(size_t nnz);
    void FreeMemory(void);
    void PrintStats(void);

public:
    daeBlockOfEquations_t*	m_pBlock;
    int                     m_nNoEquations;
    real_t*                 m_vecB;
    real_t*                 m_vecX;

    daeRawDataArray<real_t>	m_arrValues;
    daeRawDataArray<real_t>	m_arrTimeDerivatives;
    daeRawDataArray<real_t>	m_arrResiduals;
    daeSuperLUMatrix        m_matJacobian;
    bool                    m_bFactorizationDone;

    int m_omp_num_threads;

    std::map<std::string, call_stats::TimeAndCount>  m_stats;

#ifdef daeSuperLU_MT
    SuperMatrix			m_matA;
    SuperMatrix			m_matB;
    SuperMatrix			m_matX;
    SuperMatrix			m_matL;
    SuperMatrix			m_matU;
    SuperMatrix			m_matAC;

    superlumt_options_t	m_Options;
    Gstat_t				m_Stats;

    int					m_lwork;
    void*				m_work;
    int*				m_perm_c;
    int*				m_perm_r;
#endif

#ifdef daeSuperLU
    SuperMatrix			m_matA;
    SuperMatrix			m_matB;
    SuperMatrix			m_matX;
    SuperMatrix			m_matL;
    SuperMatrix			m_matU;
    SuperMatrix			m_matAC;

    superlu_options_t	m_Options;
    mem_usage_t			m_memUsage;
    SuperLUStat_t		m_Stats;
    GlobalLU_t          m_Glu;

    int*				m_etree;
    real_t*				m_R;
    real_t*				m_C;
    char				m_equed;
    real_t				m_ferr;
    real_t				m_berr;

    int					m_lwork;
    void*				m_work;
    int*				m_perm_c;
    int*				m_perm_r;

    int					m_iFactorization;
    bool				m_bUseUserSuppliedWorkSpace;
    double				m_dWorkspaceMemoryIncrement;
    double				m_dWorkspaceSizeMultiplier;
#endif

    double m_solve, m_factorize;
};

#ifdef daeSuperLU
}
#endif
#ifdef daeSuperLU_MT
}
#endif

}
}

#endif
