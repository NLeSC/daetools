#include "stdafx.h"
using namespace std;
#include "base_solvers.h"
#include "ida_solver.h"

namespace daetools 
{
namespace solver 
{

daeDAESolver_t* daeCreateIDASolver(void)
{
	return new daeIDASolver;
}

}
}
