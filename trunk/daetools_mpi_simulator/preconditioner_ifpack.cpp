/***********************************************************************************
*                 DAE Tools Project: www.daetools.com
*                 Copyright (C) Dragan Nikolic
************************************************************************************
DAE Tools is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License version 3 as published by the Free Software
Foundation. DAE Tools is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with the
DAE Tools software; if not, see <http://www.gnu.org/licenses/>.
***********************************************************************************/
#include "typedefs.h"
#include <string>
#include <iostream>
#include <vector>
#define DAE_MAJOR 1
#define DAE_MINOR 8
#define DAE_BUILD 0
#define daeSuperLU
#include "../LA_Trilinos_Amesos/trilinos_amesos_la_solver.h"

namespace daetools_mpi
{
/*
static void setParameter_int(Teuchos::ParameterList& parameters, const std::string& paramName, int defValue)
{
    daeSimulationOptions& cfg = daeSimulationOptions::GetConfig();
    std::string paramPath = "LinearSolver.Preconditioner.Parameters." + paramName;
    int value = cfg.GetInteger(paramPath, defValue);
    parameters.set(paramName, value);
}

static void setParameter_bool(Teuchos::ParameterList& parameters, const std::string& paramName, bool defValue)
{
    daeSimulationOptions& cfg = daeSimulationOptions::GetConfig();
    std::string paramPath = "LinearSolver.Preconditioner.Parameters." + paramName;
    bool value = cfg.GetBoolean(paramPath, defValue);
    parameters.set(paramName, value);
}

static void setParameter_double(Teuchos::ParameterList& parameters, const std::string& paramName, double defValue)
{
    daeSimulationOptions& cfg = daeSimulationOptions::GetConfig();
    std::string paramPath = "LinearSolver.Preconditioner.Parameters." + paramName;
    double value = cfg.GetFloat(paramPath, defValue);
    parameters.set(paramName, value);
}

static void setParameter_string(Teuchos::ParameterList& parameters, const std::string& paramName, const std::string& defValue)
{
    daeSimulationOptions& cfg = daeSimulationOptions::GetConfig();
    std::string paramPath = "LinearSolver.Preconditioner.Parameters." + paramName;
    std::string value = cfg.GetString(paramPath, defValue);
    parameters.set(paramName, value);
}
*/

class daePreconditionerData_Ifpack : public daeMatrixAccess_t
{
public:
    daePreconditionerData_Ifpack(size_t numVars, daetools_mpi::daeModel_t* mod) :
        numberOfVariables(numVars), model(mod)
    {
        m_map.reset(new Epetra_Map((int)numberOfVariables, 0, m_Comm));
        m_matEPETRA.reset(new Epetra_CrsMatrix(Copy, *m_map, 0));

        m_matJacobian.InitMatrix(numberOfVariables, m_matEPETRA.get());
        m_matJacobian.ResetCounters();
        model->GetDAESystemStructure(N, NNZ, IA, JA);
        FillSparseMatrix(&m_matJacobian);
        m_matJacobian.Sort(); // The function Sort will call FillComplete on Epetra matrix (required for Ifpack constructor).

        daeSimulationOptions& cfg = daeSimulationOptions::GetConfig();
        std::string strPreconditionerName = cfg.GetString("LinearSolver.Preconditioner.Name", "ILU");

        Ifpack factory;
        Ifpack_Preconditioner* preconditioner = factory.Create(strPreconditionerName, m_matEPETRA.get());
        if(!preconditioner)
            daeThrowException("Failed to create Ifpack preconditioner " + strPreconditionerName);

        m_pPreconditionerIfpack.reset(preconditioner);
        printf("Instantiated %s preconditioner (requested: %s)\n", preconditioner->Label(), strPreconditionerName.c_str());

        Teuchos::ParameterList parameters;
        // This will set the default parameters.
        m_pPreconditionerIfpack->SetParameters(parameters);

        printf("Default Ifpack parameters:\n");
        parameters.print(std::cout, 2, true, true);
        printf("\n");

        /*
        if(strPreconditionerName == "ILU")
        {
            setParameter_int   (parameters, "fact: level-of-fill",      0);
            setParameter_double(parameters, "fact: relax value",        0.0);
            setParameter_double(parameters, "fact: absolute threshold", 1E-5);
            setParameter_double(parameters, "fact: relative threshold", 1.00);
        }
        else if(strPreconditionerName == "ILUT")
        {
            setParameter_double(parameters, "fact: ilut level-of-fill",  3.0);
            setParameter_double(parameters, "fact: relax value",         0.0);
            setParameter_double(parameters, "fact: absolute threshold",  1E-5);
            setParameter_double(parameters, "fact: relative threshold",  1.00);
            setParameter_double(parameters, "fact: drop tolerance",      0.0);
        }
        else
        {
            daeThrowException("Invalid Ifpack preconditioner specified: " + strPreconditionerName);
        }
        */

        // Iterate over the default list of parameters and update them (if set in the simulation options).
        printf("Processing Ifpack parameters from '%s' ...\n", cfg.configFile.c_str());
        for(Teuchos::ParameterList::ConstIterator it = parameters.begin(); it != parameters.end(); it++)
        {
            std::string             paramName = it->first;
            Teuchos::ParameterEntry pe        = it->second;
            Teuchos::any            anyValue  = pe.getAny();

            std::string paramPath = "LinearSolver.Preconditioner.Parameters." + paramName;
            if(cfg.HasKey(paramPath))
            {
                if(anyValue.type() == typeid(int))
                {
                    int value = cfg.GetInteger(paramPath);
                    parameters.set(paramName, value);
                    printf("  Found parameter: %s = %d\n", paramName.c_str(), value);
                }
                else if(anyValue.type() == typeid(double))
                {
                    double value = cfg.GetFloat(paramPath);
                    parameters.set(paramName, value);
                    printf("  Found parameter: %s = %f\n", paramName.c_str(), value);
                }
                else if(anyValue.type() == typeid(bool))
                {
                    bool value = cfg.GetBoolean(paramPath);
                    parameters.set(paramName, value);
                    printf("  Found parameter: %s = %d\n", paramName.c_str(), value);
                }
                else if(anyValue.type() == typeid(std::string))
                {
                    std::string value = cfg.GetString(paramPath);
                    parameters.set(paramName, value);
                    printf("  Found parameter: %s = %s\n", paramName.c_str(), value.c_str());
                }
            }
        }
        printf("\n");

        m_pPreconditionerIfpack->SetParameters(parameters);
        m_pPreconditionerIfpack->Initialize();

        printf("Proceed with Ifpack parameters:\n");
        parameters.print(std::cout, 2, true, true);
        printf("\n");
    }

    void FillSparseMatrix(dae::daeSparseMatrix<real_t>* pmatrix)
    {
        if(numberOfVariables != N)
            throw std::runtime_error("");
        if(numberOfVariables+1 != IA.size())
            throw std::runtime_error("");
        if(JA.size() != NNZ)
            throw std::runtime_error("");

        std::map<size_t, size_t> mapIndexes;
        for(int row = 0; row < numberOfVariables; row++)
        {
            int colStart = IA[row];
            int colEnd   = IA[row+1];
            mapIndexes.clear();
            for(int col = colStart; col < colEnd; col++)
            {
                size_t bi = JA[col];
                mapIndexes[bi] = bi;
            }
            pmatrix->AddRow(mapIndexes);
        }
    }

    void SetItem(size_t row, size_t col, real_t value)
    {
        m_matJacobian.SetItem(row, col, value);
    }

public:
    int                                                 N;
    int                                                 NNZ;
    std::vector<int>                                    IA;
    std::vector<int>                                    JA;
    size_t                                              numberOfVariables;
    daetools_mpi::daeModel_t*                           model;

    boost::shared_ptr<Epetra_Map>                       m_map;
    boost::shared_ptr<Epetra_CrsMatrix>                 m_matEPETRA;
    Epetra_SerialComm                                   m_Comm;
    boost::shared_ptr<Ifpack_Preconditioner>			m_pPreconditionerIfpack;
    dae::solver::daeEpetraCSRMatrix                     m_matJacobian;
};

daePreconditioner_Ifpack::daePreconditioner_Ifpack()
{
    data = NULL;
}

daePreconditioner_Ifpack::~daePreconditioner_Ifpack()
{
    daePreconditionerData_Ifpack* p_data = (daePreconditionerData_Ifpack*)this->data;
    if(p_data)
    {
        printf("ComputeTime      = %.2f\n", p_data->m_pPreconditionerIfpack->ComputeTime());
        printf("ApplyInverseTime = %.2f\n", p_data->m_pPreconditionerIfpack->ApplyInverseTime());
        printf("InitializeTime   = %.2f\n", p_data->m_pPreconditionerIfpack->InitializeTime());
    }
    Free();
}

int daePreconditioner_Ifpack::Initialize(daetools_mpi::daeModel_t *model, size_t numberOfVariables)
{
    daePreconditionerData_Ifpack* p_data = new daePreconditionerData_Ifpack(numberOfVariables, model);
    this->data = p_data;

    return 0;
}

int daePreconditioner_Ifpack::Setup(real_t  time,
                                    real_t  inverseTimeStep,
                                    real_t* values,
                                    real_t* timeDerivatives,
                                    real_t* residuals)
{
    daePreconditionerData_Ifpack* p_data = (daePreconditionerData_Ifpack*)this->data;

    p_data->m_matJacobian.ClearMatrix();
    daeMatrixAccess_t* ma = p_data;
    p_data->model->EvaluateJacobian(p_data->numberOfVariables, time,  inverseTimeStep, values, timeDerivatives, residuals, ma);

    int ret = p_data->m_pPreconditionerIfpack->Compute();
    printf("    t = %.15f compute preconditioner (condest = %.2e)\n", time, p_data->m_pPreconditionerIfpack->Condest());

    return ret;
}

int daePreconditioner_Ifpack::Solve(real_t  time, real_t* r, real_t* z)
{
    daePreconditionerData_Ifpack* p_data = (daePreconditionerData_Ifpack*)this->data;

    Epetra_MultiVector vecR(View, *p_data->m_map.get(), &r, 1);
    Epetra_MultiVector vecZ(View, *p_data->m_map.get(), &z, 1);

    return p_data->m_pPreconditionerIfpack->ApplyInverse(vecR, vecZ);
}

int daePreconditioner_Ifpack::JacobianVectorMultiply(real_t  time, real_t* v, real_t* Jv)
{
    daePreconditionerData_Ifpack* p_data = (daePreconditionerData_Ifpack*)this->data;

    Epetra_MultiVector vecV (View, *p_data->m_map.get(), &v,  1);
    Epetra_MultiVector vecJv(View, *p_data->m_map.get(), &Jv, 1);

    return p_data->m_matEPETRA->Multiply(false, vecV, vecJv);
}

int daePreconditioner_Ifpack::Free()
{
    daePreconditionerData_Ifpack* p_data = (daePreconditionerData_Ifpack*)this->data;

    if(p_data)
    {
        delete p_data;
        data = NULL;
    }

    return 0;
}

}
