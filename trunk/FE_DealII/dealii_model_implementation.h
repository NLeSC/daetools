#ifndef DEAL_II_MODEL_IMPLEMENTATION_H
#define DEAL_II_MODEL_IMPLEMENTATION_H

#include <typeinfo>
#include <fstream>
#include <iostream>
#include <iterator>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "dealii_fe_model.h"

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/base/logstream.h>
#include <deal.II/lac/vector.h>
#include <deal.II/lac/vector.templates.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/full_matrix.templates.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/sparse_matrix.templates.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/constraint_matrix.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_refinement.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/tria_boundary_lib.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/error_estimator.h>
#include <deal.II/numerics/data_out.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_in.h>

#include <deal.II/dofs/dof_renumbering.h>
#include <deal.II/base/smartpointer.h>
#include <deal.II/numerics/vector_tools.h>
#include <deal.II/base/convergence_table.h>
#include <deal.II/fe/fe_values.h>

namespace dae
{
namespace fe_solver
{
/******************************************************************
    dealiiModel<dim>
*******************************************************************/
using namespace dealii;

template <int dim>
class dealiiModel : public daeFiniteElementsModel_dealII
{
typedef typename std::map<unsigned int, const dealiiFunction<dim>*> map_Uint_FunctionPtr;
typedef typename std::map<std::string,  const dealiiFunction<dim>*> map_String_FunctionPtr;

public:
    dealiiModel(const std::string&            meshFilename,
                const std::string&            quadratureFormula,
                unsigned int                  polynomialOrder,
                const map_String_FunctionPtr& functions,
                const map_Uint_FunctionPtr&   dirichletBC,
                const map_Uint_FunctionPtr&   neumannBC);
    
    virtual ~dealiiModel();
    
public:
    virtual void setup_system();
    virtual void assemble_system();
    virtual void finalize_solution_and_save(const std::string& strOutputDirectory,
                                            const std::string& strFormat,
                                            const std::string& strVariableName,
                                            const double* values,
                                            size_t n,
                                            double time);

public:
    // Additional deal.II specific data
    Triangulation<dim>                   triangulation;
    DoFHandler<dim>                      dof_handler;
    SmartPointer< FiniteElement<dim> >   fe;
    ConstraintMatrix                     hanging_node_constraints;

    // Model-specific data
    std::string             m_quadrature_formula;
    map_String_FunctionPtr  funsFunctions;
    map_Uint_FunctionPtr    funsDirichletBC;
    map_Uint_FunctionPtr    funsNeumannBC;
    int                     m_outputCounter;
};

template <int dim>
dealiiModel<dim>::dealiiModel (const std::string&            meshFilename,
                               const std::string&            quadratureFormula,
                               unsigned int                  polynomialOrder,
                               const map_String_FunctionPtr& functions,
                               const map_Uint_FunctionPtr&   dirichletBC,
                               const map_Uint_FunctionPtr&   neumannBC):
    dof_handler (triangulation),
    fe (new FE_Q<dim>(polynomialOrder)),
    m_quadrature_formula(quadratureFormula),
    funsFunctions(functions),
    funsDirichletBC(dirichletBC),
    funsNeumannBC(neumannBC),
    m_outputCounter(0)
{

    GridIn<dim> gridin;
    gridin.attach_triangulation(triangulation);
    std::ifstream f(meshFilename);
    gridin.read_msh(f);

    dealiiModel<dim>::setup_system();
}

template <int dim>
dealiiModel<dim>::~dealiiModel ()
{
    dof_handler.clear ();
}

template <int dim>
void dealiiModel<dim>::setup_system()
{
    dof_handler.distribute_dofs (*fe);
    DoFRenumbering::Cuthill_McKee (dof_handler);

    hanging_node_constraints.clear ();
    DoFTools::make_hanging_node_constraints (dof_handler,
                                             hanging_node_constraints);
    hanging_node_constraints.close ();

    sparsity_pattern.reinit (dof_handler.n_dofs(),
                             dof_handler.n_dofs(),
                             dof_handler.max_couplings_between_dofs());
    DoFTools::make_sparsity_pattern (dof_handler, sparsity_pattern);
    hanging_node_constraints.condense (sparsity_pattern);
    sparsity_pattern.compress();

    system_matrix.reinit (sparsity_pattern);
    system_matrix_dt.reinit (sparsity_pattern);
    solution.reinit (dof_handler.n_dofs());
    system_rhs.reinit (dof_handler.n_dofs());

/* Old code
    dof_handler.distribute_dofs (*fe);
    
    SparsityPattern sparsity_pattern_pre;
    sparsity_pattern_pre.reinit (dof_handler.n_dofs(),
                                 dof_handler.n_dofs(),
                                 dof_handler.max_couplings_between_dofs());
    DoFTools::make_sparsity_pattern (dof_handler, sparsity_pattern_pre);
    hanging_node_constraints.condense (sparsity_pattern_pre);
    sparsity_pattern_pre.compress();

    std::ofstream out1 ("sparsity_pattern_pre");
    sparsity_pattern_pre.print_gnuplot (out1);

    DoFRenumbering::Cuthill_McKee (dof_handler);
    
    hanging_node_constraints.clear ();
    DoFTools::make_hanging_node_constraints (dof_handler,
                                             hanging_node_constraints);
    hanging_node_constraints.close ();
    
    sparsity_pattern.reinit (dof_handler.n_dofs(),
                             dof_handler.n_dofs(),
                             dof_handler.max_couplings_between_dofs());
    DoFTools::make_sparsity_pattern (dof_handler, sparsity_pattern);
    hanging_node_constraints.condense (sparsity_pattern);
    sparsity_pattern.compress();

    // Global matrices
    system_matrix.reinit (sparsity_pattern);
    system_matrix_dt.reinit (sparsity_pattern);
    system_rhs.reinit (dof_handler.n_dofs());
    solution.reinit(dof_handler.n_dofs());

    std::ofstream out ("sparsity_pattern_after");
    sparsity_pattern.print_gnuplot (out);
*/
}

template <int dim>
void dealiiModel<dim>::assemble_system()
{
    if(m_quadrature_formula == "")
    {
    }

    QGauss<dim>   quadrature_formula(3);
    QGauss<dim-1> face_quadrature_formula(3);
    
    const unsigned int n_q_points      = quadrature_formula.size();
    const unsigned int n_face_q_points = face_quadrature_formula.size();
    
    const unsigned int dofs_per_cell = fe->dofs_per_cell;
    
    FullMatrix<double>  cell_matrix(dofs_per_cell, dofs_per_cell);
    FullMatrix<double>  cell_matrix_dt(dofs_per_cell, dofs_per_cell);
    Vector<double>      cell_rhs(dofs_per_cell);

    std::vector<unsigned int> local_dof_indices (dofs_per_cell);
    
    FEValues<dim>  fe_values (*fe, quadrature_formula,
                              update_values   | update_gradients |
                              update_quadrature_points | update_JxW_values);
    
    FEFaceValues<dim> fe_face_values (*fe, face_quadrature_formula,
                                      update_values         | update_quadrature_points  |
                                      update_normal_vectors | update_JxW_values);

    if(funsFunctions.find("Diffusivity") == funsFunctions.end() || !funsFunctions.find("Diffusivity")->second)
        throw std::runtime_error("Invalid function: Diffusivity");
    if(funsFunctions.find("Generation") == funsFunctions.end() || !funsFunctions.find("Generation")->second)
        throw std::runtime_error("Invalid function: Generation");
    if(funsFunctions.find("Velocity") == funsFunctions.end() || !funsFunctions.find("Velocity")->second)
        throw std::runtime_error("Invalid function: Velocity");

    const dealiiFunction<dim>* funDiffusivity = funsFunctions.find("Diffusivity")->second;
    const dealiiFunction<dim>* funGeneration  = funsFunctions.find("Generation")->second;
    const dealiiFunction<dim>* funVelocity    = funsFunctions.find("Velocity")->second;

    // All DOFs at the boundary ID that have Dirichlet BCs imposed.
    // mapDirichlets: map< boundary_id, map<dof, value> > will be used to apply boundary conditions locally.
    // We build this map here since it is common for all cells.
    /*
    std::map< unsigned int, std::map<unsigned int, double> > mapDirichlets;
    for(typename std::map< unsigned int, dealiiFunction<dim> >::const_iterator it = funsDirichletBC.begin(); it != funsDirichletBC.end(); it++)
    {
        const unsigned int id          = it->first;
        const dealiiFunction<dim>& fun = it->second;

        std::map<unsigned int,double> boundary_values;
        VectorTools::interpolate_boundary_values (dof_handler,
                                                  id,
                                                  fun,
                                                  boundary_values);

        mapDirichlets[id] = boundary_values;
    }
    */

    /*
    // All DOFs at the boundary ID that have Neumann BCs imposed
    // mapNeumanns: map< boundary_id, map<global_dof_index, value> >
    std::map< unsigned int, std::map<unsigned int,double> > mapNeumanns;
    for(std::map<unsigned int, double>::iterator it = NeumanBC.begin(); it != NeumanBC.end(); it++)
    {
        const unsigned int id      = it->first;
        const double neumann_value = it->second;

        std::map<unsigned int,double> boundary_values;
        VectorTools::interpolate_boundary_values (dof_handler,
                                                  id,
                                                  SingleValue_Function<dim>(neumann_value),
                                                  boundary_values);

        mapNeumanns[id] = boundary_values;
    }
    */

    typename DoFHandler<dim>::active_cell_iterator cell = dof_handler.begin_active(),
                                                   endc = dof_handler.end();
    for(int cellCounter = 0; cell != endc; ++cell, ++cellCounter)
    {
        cell_matrix    = 0;
        cell_matrix_dt = 0;
        cell_rhs       = 0;
        
        fe_values.reinit(cell);
        cell->get_dof_indices(local_dof_indices);

        for(unsigned int q_point = 0; q_point < n_q_points; ++q_point)
        {
            for(unsigned int i = 0; i < dofs_per_cell; ++i)
            {
                for(unsigned int j = 0; j < dofs_per_cell; ++j)
                {
                    cell_matrix(i,j)    += (
                                             /* Diffusion */
                                                (
                                                   fe_values.shape_grad(i, q_point) *
                                                   fe_values.shape_grad(j, q_point)
                                                )
                                                *
                                                funDiffusivity->value(fe_values.quadrature_point(q_point))
                                                +
                                             /* Helmholtz term (u) */
                                                fe_values.shape_value(i, q_point) *
                                                fe_values.shape_value(j, q_point)

                                             /* Convection (nothing at the moment) */
                                                /* +
                                                fe_values.shape_value(i, q_point) *
                                                fe_values.shape_value(j, q_point) *
                                                funVelocity.value(fe_values.quadrature_point(q_point)) */
                                           )
                                           *
                                           fe_values.JxW(q_point);

                    /* Accumulation (in a separate matrix) */
                    cell_matrix_dt(i,j) += (
                                              fe_values.shape_value(i, q_point) *
                                              fe_values.shape_value(j, q_point)
                                           )
                                           *
                                           fe_values.JxW(q_point);
                }
                
                /* Generation */
                cell_rhs(i) +=  fe_values.shape_value(i,q_point) *
                                funGeneration->value(fe_values.quadrature_point(q_point), 0) *
                                fe_values.JxW(q_point);
            }
        }

        for(unsigned int face = 0; face<GeometryInfo<dim>::faces_per_cell; ++face)
        {
            if(cell->face(face)->at_boundary())
            {
                fe_face_values.reinit (cell, face);

                const unsigned int id = cell->face(face)->boundary_indicator();

                if(funsDirichletBC.find(id) != funsDirichletBC.end())
                {
                    // Dirichlet BC
                    // Do nothing now; apply boundary conditions on the final system_matrix and system_rhs only at the end of the assembling
                }
                else if(funsNeumannBC.find(id) != funsNeumannBC.end())
                {
                    // Neumann BC
                    const dealiiFunction<dim>& neumann = *funsNeumannBC.find(id)->second;

                    std::cout << (boost::format("  Setting NeumanBC (cell=%d, face=%d, id= %d)[q0] = %f") % cellCounter % face % id % neumann.value(fe_face_values.quadrature_point(0))).str() << std::endl;

                    for(unsigned int q_point = 0; q_point < n_face_q_points; ++q_point)
                    {
                        // Achtung, Achtung! For the Convection-Diffusion-Reaction system only:
                        //                   the sign '-neumann' since we have the term: -integral(q * φ(i) * dΓq)
                        for (unsigned int i = 0; i < dofs_per_cell; ++i)
                            cell_rhs(i) += neumann.value(fe_face_values.quadrature_point(q_point))
                                           *
                                           fe_face_values.shape_value(i, q_point)
                                           *
                                           fe_face_values.JxW(q_point);
                    }
                }
                else
                {
                    // Not found
                    // Do nothing or do some default action (perhaps zero gradient?)
                }
            }
        }

        /* ACHTUNG, ACHTUNG!!
           Apply Dirichlet boundary conditions locally (conflicts with refined grids with hanging nodes!!)

        // We already have a pre-calculated map<global_dof_index, bc_value> for every ID marked as having Dirichlet BCs imposed
        for(std::map< unsigned int, std::map<unsigned int, double> >::iterator it = mapDirichlets.begin(); it != mapDirichlets.end(); it++)
        {
            unsigned int id                                 = it->first;
            std::map<unsigned int, double>& boundary_values = it->second;

            // Print some mumbo-jumbo voodoo-mojo stuf related to cell_matrix and cell_rhs...
            std::cout << "boundary_values" << std::endl;
            for(std::map<unsigned int,double>::iterator bviter = boundary_values.begin(); bviter != boundary_values.end(); bviter++)
                std::cout << "(" << bviter->first << ", " << bviter->second << ") ";
            std::cout << std::endl;

            std::cout << "local_dof_indices" << std::endl;
            for(std::vector<unsigned int>::iterator ldiiter = local_dof_indices.begin(); ldiiter != local_dof_indices.end(); ldiiter++)
                std::cout << *ldiiter << " ";
            std::cout << std::endl;

            std::cout << "cell_matrix before bc:" << std::endl;
            cell_matrix.print_formatted(std::cout);
            std::cout << "cell_rhs before bc:" << std::endl;
            cell_rhs.print(std::cout);

            // Apply values to the cell_matrix and cell_rhs
            MatrixTools::local_apply_boundary_values(boundary_values,
                                                     local_dof_indices,
                                                     cell_matrix,
                                                     cell_rhs,
                                                     true);

            std::cout << "cell_matrix after bc:" << std::endl;
            cell_matrix.print_formatted(std::cout);
            std::cout << "cell_rhs after bc:" << std::endl;
            cell_rhs.print(std::cout);
        }
        */

        // Add to the system matrices/vector
        for(unsigned int i = 0; i < dofs_per_cell; ++i)
        {
            for(unsigned int j = 0; j < dofs_per_cell; ++j)
            { 
                system_matrix.add   (local_dof_indices[i], local_dof_indices[j], cell_matrix(i,j));
                system_matrix_dt.add(local_dof_indices[i], local_dof_indices[j], cell_matrix_dt(i,j));
            }
            system_rhs(local_dof_indices[i]) += cell_rhs(i);
        }
    }

    // If using refined grids condense hanging nodes
    hanging_node_constraints.condense(system_matrix);
    hanging_node_constraints.condense(system_rhs);

    // What about this matrix? Should it also be condensed?
    //hanging_node_constraints.condense(system_matrix_dt);

    // Apply Dirichlet boundary conditions on the system matrix and rhs
    for(typename map_Uint_FunctionPtr::const_iterator it = funsDirichletBC.begin(); it != funsDirichletBC.end(); it++)
    {
        const unsigned int id          =  it->first;
        const dealiiFunction<dim>& fun = *it->second;
        std::cout << "Setting DirichletBC at id " << id << " with sample value " << fun.value(Point<dim>(0,0,0)) << std::endl;

        std::map<types::global_dof_index, double> boundary_values;
        VectorTools::interpolate_boundary_values (dof_handler,
                                                  id,
                                                  fun,
                                                  boundary_values);
        MatrixTools::apply_boundary_values (boundary_values,
                                            system_matrix,
                                            solution,
                                            system_rhs);
    }
}

template <int dim>
void dealiiModel<dim>::finalize_solution_and_save(const std::string& strOutputDirectory,
                                                  const std::string& strFormat,
                                                  const std::string& strVariableName,
                                                  const double* values,
                                                  size_t n,
                                                  double time)
{
    std::cout << "finalize_solution_and_save" << std::endl;
    boost::filesystem::path vtkFilename((boost::format("%4d. %s(t=%f).vtk") % m_outputCounter
                                                                            % strVariableName
                                                                            % time).str());
    boost::filesystem::path vtkPath(strOutputDirectory);
    vtkPath /= vtkFilename;

    for(size_t i = 0; i < n; i++)
        solution[i] = values[i];

    // We may call distribute() on solution to fix hanging nodes
    std::cout << "solution after solve:" << solution << std::endl;
    hanging_node_constraints.distribute(solution);
    std::cout << "solution after distribute:" << solution << std::endl;

    //std::cout << "strVariableName = " << strVariableName << std::endl;
    //std::cout << "strFileName = "     << strFileName << std::endl;
    //std::cout << "solution:" << std::endl;
    //for(size_t i = 0; i < value->m_nNumberOfPoints; i++)
    //    std::cout << solution[i] << " ";
    //std::cout << std::endl;
    //std::cout << "Report: " << value->m_strName << " at " << time  << std::endl;

    DataOut<dim> data_out;
    data_out.attach_dof_handler(dof_handler);
    data_out.add_data_vector(solution, strVariableName);
    data_out.build_patches(fe->degree);
    std::ofstream output(vtkPath.c_str());
    data_out.write_vtk(output);

    m_outputCounter++;
}

}
}

#endif