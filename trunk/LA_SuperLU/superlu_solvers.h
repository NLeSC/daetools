#ifndef DAE_SUPERLU_SOLVERS_H
#define DAE_SUPERLU_SOLVERS_H
#include "../Core/solver.h"
//#include "../IDAS_DAESolver/ida_la_solver_interface.h"

#if defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(_WIN64)

#ifdef DAE_DLL_INTERFACE
#ifdef SUPERLU_EXPORTS
#define DAE_SUPERLU_API __declspec(dllexport)
#else
#define DAE_SUPERLU_API __declspec(dllimport)
#endif
#else // DAE_DLL_INTERFACE
#define DAE_SUPERLU_API
#endif // DAE_DLL_INTERFACE

#else // WIN32
#define DAE_SUPERLU_API
#endif // WIN32


namespace dae
{
namespace solver
{
#ifdef daeSuperLU
DAE_SUPERLU_API daeLASolver_t* daeCreateSuperLUSolver(void);
#endif

#ifdef daeSuperLU_MT
DAE_SUPERLU_API daeLASolver_t* daeCreateSuperLU_MTSolver(void);
#endif
}
}

#endif
