/***********************************************************************************
                 OpenCS Project: www.daetools.com
                 Copyright (C) Dragan Nikolic
************************************************************************************
OpenCS is free software; you can redistribute it and/or modify it under the terms of
the GNU Lesser General Public License version 3 as published by the Free Software
Foundation. OpenCS is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with
the OpenCS software; if not, see <http://www.gnu.org/licenses/>.
***********************************************************************************/
#include <string>
#include <iostream>
#include <vector>
#include "daesimulator.h"
#include "trilinos_solver.h"

namespace cs_dae_simulator
{
class daePreconditionerData_ML : public cs::csMatrixAccess_t
{
public:
    daePreconditionerData_ML(size_t numVars, cs::csDifferentialEquationModel_t* mod) :
        numberOfVariables(numVars), model(mod)
    {
        m_map.reset(new Epetra_Map((int)numberOfVariables, 0, m_Comm));
        m_matEPETRA.reset(new Epetra_CrsMatrix(Copy, *m_map, 0));

        m_diagonal.reset(new Epetra_Vector(*m_map, true));
        if(numberOfVariables != m_diagonal->GlobalLength())
            csThrowException("Diagonal array length mismatch");

        m_matJacobian.InitMatrix(numberOfVariables, m_matEPETRA.get());
        m_matJacobian.ResetCounters();
        model->GetSparsityPattern(N, NNZ, IA, JA);
        FillSparseMatrix(&m_matJacobian);
        m_matJacobian.Sort(); // The function Sort will call FillComplete on Epetra matrix (required for Ifpack constructor).

        daeSimulationOptions& cfg = daeSimulationOptions::GetConfig();
        printInfo = cfg.GetBoolean("LinearSolver.Preconditioner.PrintInfo", false);

        std::string strPreconditionerName = cfg.GetString("LinearSolver.Preconditioner.Name", "SA");

        if(strPreconditionerName != "SA"       &&
           strPreconditionerName != "DD"       &&
           strPreconditionerName != "DD-ML"    &&
           strPreconditionerName != "DD-ML-LU" &&
           strPreconditionerName != "maxwell"  &&
           strPreconditionerName != "NSSA")
        {
            csThrowException("Invalid preconditioner: " + strPreconditionerName);
        }

        Teuchos::ParameterList parameters;
        ML_Epetra::SetDefaults(strPreconditionerName, parameters);
        parameters.set("reuse: enable", true);
        if(printInfo)
        {
            printf("Default ML parameters:\n");
            parameters.print(std::cout, 2, true, true);
            printf("\n");
        }
        // Iterate over the default list of parameters and update them (if set in the simulation options).
        if(printInfo)
            printf("Processing ML parameters from '%s' ...\n", cfg.configFile.c_str());
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
                    if(printInfo)
                        printf("  Found parameter: %s = %d\n", paramName.c_str(), value);
                }
                else if(anyValue.type() == typeid(double))
                {
                    double value = cfg.GetFloat(paramPath);
                    parameters.set(paramName, value);
                    if(printInfo)
                        printf("  Found parameter: %s = %f\n", paramName.c_str(), value);
                }
                else if(anyValue.type() == typeid(bool))
                {
                    bool value = cfg.GetBoolean(paramPath);
                    parameters.set(paramName, value);
                    if(printInfo)
                        printf("  Found parameter: %s = %d\n", paramName.c_str(), value);
                }
                else if(anyValue.type() == typeid(std::string))
                {
                    std::string value = cfg.GetString(paramPath);
                    parameters.set(paramName, value);
                    if(printInfo)
                        printf("  Found parameter: %s = %s\n", paramName.c_str(), value.c_str());
                }
            }
        }
        if(printInfo)
            printf("\n");

        ML_Epetra::MultiLevelPreconditioner* preconditioner = new ML_Epetra::MultiLevelPreconditioner(*m_matEPETRA.get(), parameters, false);
        if(preconditioner == NULL)
            csThrowException("Failed to create ML preconditioner " + strPreconditionerName);

        m_pPreconditionerML.reset(preconditioner);

        if(printInfo)
        {
            printf("Proceed with ML parameters:\n");
            parameters.print(std::cout, 2, true, true);
            printf("\n");
        }
    }

    void FillSparseMatrix(daetools::daeSparseMatrix<real_t>* pmatrix)
    {
        if(numberOfVariables != N)
            csThrowException("");
        if(numberOfVariables+1 != IA.size())
            csThrowException("");
        if(JA.size() != NNZ)
            csThrowException("");

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
    bool                                                printInfo;
    int                                                 N;
    int                                                 NNZ;
    std::vector<int>                                    IA;
    std::vector<int>                                    JA;
    size_t                                              numberOfVariables;
    cs::csDifferentialEquationModel_t*                  model;

    std::shared_ptr<Epetra_Map>                             m_map;
    std::shared_ptr<Epetra_Vector>                          m_diagonal;
    std::shared_ptr<Epetra_CrsMatrix>                       m_matEPETRA;
    Epetra_SerialComm                                       m_Comm;
    std::shared_ptr<ML_Epetra::MultiLevelPreconditioner>	m_pPreconditionerML;
    daetools::solver::daeEpetraCSRMatrix                         m_matJacobian;
};

daePreconditioner_ML::daePreconditioner_ML()
{
}

daePreconditioner_ML::~daePreconditioner_ML()
{
    Free();
}

int daePreconditioner_ML::Initialize(cs::csDifferentialEquationModel_t* model,
                                     size_t numberOfVariables,
                                     bool   isODESystem_)
{
    isODESystem = isODESystem_;
    daePreconditionerData_ML* p_data = new daePreconditionerData_ML(numberOfVariables, model);
    this->data = p_data;

    return 0;
}

int daePreconditioner_ML::Setup(real_t  time,
                                real_t  inverseTimeStep,
                                real_t* values,
                                real_t* timeDerivatives,
                                bool    recomputeJacobian,
                                real_t  jacobianScaleFactor)
{
    daePreconditionerData_ML* p_data = (daePreconditionerData_ML*)this->data;

    // Calling SetAndSynchroniseData is not required here since it has previously been called by residuals function.

    // Here, we do not use recomputeJacobian since if we update the Jacobian with [I] - gamma*[Jrhs]
    //   we cannot do it again with the different gamma.
    // Therefore, always compute the Jacobian.
    // Double check about the performance penalty it might cause!!
    p_data->m_matJacobian.ClearMatrix();
    cs::csMatrixAccess_t* ma = p_data;
    p_data->model->EvaluateJacobian(time, inverseTimeStep, ma);

    // The Jacobian matrix we calculated is the Jacobian for the system of equations.
    // For ODE systems it is the RHS Jacobian (Jrhs) not the full system Jacobian.
    // The Jacobian matrix for ODE systems is in the form: [J] = [I] - gamma * [Jrhs],
    //   where gamma is a scaling factor sent by the ODE solver.
    // Here, the full Jacobian must be calculated.
    if(isODESystem)
    {
        // (1) Scale the Jrhs with the -gamma factor:
        p_data->m_matEPETRA->Scale(-jacobianScaleFactor);

        // (2) Extract the diagonal items and repleace them with diag[i] = 1 + diag[i]:
        int ret = p_data->m_matEPETRA->ExtractDiagonalCopy(*p_data->m_diagonal);
        if(ret != 0)
            csThrowException("Failed to extract the diagonal copy of the Jacobian matrix.");

        for(uint32_t i = 0; i < p_data->numberOfVariables; i++)
            (*p_data->m_diagonal)[i] = 1.0 + (*p_data->m_diagonal)[i];

        // (3) Put the modified diagonal back to the Epetra matrix.
        ret = p_data->m_matEPETRA->ReplaceDiagonalValues(*p_data->m_diagonal);
        if(ret != 0)
            csThrowException("Failed to replace the diagonal items in the Jacobian matrix.");
    }

    // Compute the preconditioner.
    if(p_data->m_pPreconditionerML->IsPreconditionerComputed())
        p_data->m_pPreconditionerML->DestroyPreconditioner();

    int ret = p_data->m_pPreconditionerML->ComputePreconditioner();

    if(p_data->printInfo)
    {
        Teuchos::ParameterList& l = p_data->m_pPreconditionerML->GetList();
        l.print(std::cout, 2, true, true);
    }
    return ret;
}

int daePreconditioner_ML::Solve(real_t  time, real_t* r, real_t* z)
{
    daePreconditionerData_ML* p_data = (daePreconditionerData_ML*)this->data;

    Epetra_MultiVector vecR(View, *p_data->m_map.get(), &r, 1);
    Epetra_MultiVector vecZ(View, *p_data->m_map.get(), &z, 1);

    p_data->m_pPreconditionerML->ApplyInverse(vecR, vecZ);
    //p_data->m_pPreconditionerML->ReportTime();

    return 0;
}

int daePreconditioner_ML::JacobianVectorMultiply(real_t  time, real_t* v, real_t* Jv)
{
    daePreconditionerData_ML* p_data = (daePreconditionerData_ML*)this->data;

    Epetra_MultiVector vecV (View, *p_data->m_map.get(), &v,  1);
    Epetra_MultiVector vecJv(View, *p_data->m_map.get(), &Jv, 1);

    return p_data->m_matEPETRA->Multiply(false, vecV, vecJv);
}

int daePreconditioner_ML::Free()
{
    daePreconditionerData_ML* p_data = (daePreconditionerData_ML*)this->data;

    if(p_data)
    {
        delete p_data;
        data = NULL;
    }

    return 0;
}

}
