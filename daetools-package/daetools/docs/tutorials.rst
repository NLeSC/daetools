**********
Tutorials
**********
..  
    Copyright (C) Dragan Nikolic
    DAE Tools is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 3 as published by the Free Software
    Foundation. DAE Tools is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along with the
    DAE Tools software; if not, see <http://www.gnu.org/licenses/>.


.. _tutorials_basic:

.. rubric:: Basic Tutorials

The `Basic Tutorials <tutorials-basic.html>`_ illustrate the fundamental modelling concepts in **DAE Tools**,
discontinuous equations and state transition networks, user-defined schedules, simulation options, 
use of data reporter, data receiver and log objects, DAE and LA solvers, interoperability with NumPy, 
support for discrete systems, external functions, thermophysical property packages and variable constraints.


.. _tutorials_advanced:

.. rubric:: Advanced Tutorials

The `Advanced Tutorials <tutorials-advanced.html>`_ illustrate the advanced **DAE Tools**
features such as solution of the discretized population balance equations using high resolution
upwind schemes with flux limiter, modelling of lithium-ion battery based on porous electrode theory,
interactive operating procedures, code-generation/co-simulation/model-exchange capabilities.

.. _tutorials_code_verification:

.. rubric:: Code Verification Tests

`Code Verification Tests <tutorials-cv.html>`_ using the Method of Exact Solutions and the Method of Manufactured Solutions.

.. _tutorials_fe:

.. rubric:: Finite Element Tutorials

The `Finite Element Tutorials <tutorials-fe.html>`_ illustrate the support for finite element
method in **DAE Tools** through the deal.II library.


.. _tutorials_chemeng:

.. rubric:: Chemical Engineering Examples

The `Chemical Engineering Examples <tutorials-chemeng.html>`_ illustrate various chemical engineering
problems solved by **DAE Tools**.


.. _tutorials_sensitivity_analysis:

.. rubric:: Sensitivity Analysis Examples

`Sensitivity Analysis Examples <tutorials-sa.html>`_ using the local and global sensitivity analysis methods: 

- Derivative-based method (local)
- Elementary Effects method (Moris)
- Variance based methods (FAST and Sobol)


.. _tutorials_opecs:

.. rubric:: OpenCS Examples

The `OpenCS Examples <tutorials-opencs.html>`_ illustrate the capabilities of the **OpenCS** framework (using Python wrappers).


.. _tutorials_optimisation:

.. rubric:: Optimisation Tutorials

The `Optimisation Tutorials <tutorials-optimisation.html>`_ illustrate **DAE Tools** capabilities
to solve NLP/MINLP optimisation and parameter estimation problems.


.. _tutorials_chemeng_optimisation:

.. rubric:: Chemical Engineering Optimisation Examples

The `Chemical Engineering Optimisation Examples <tutorials-chemeng-optimisation.html>`_ illustrate various
chemical engineering optimisation problems solved by **DAE Tools**. This section contains 5
`Constrained Optimization Problem Set (COPS) <http://www.mcs.anl.gov/~more/cops>`_ optimisation,
parameter estimation and optimal control tests.


.. rubric:: The full list of tutorials:

.. toctree::
   :numbered:
   :maxdepth: 2 

   tutorials-basic
   tutorials-advanced
   tutorials-cv
   tutorials-fe
   tutorials-chemeng
   tutorials-sa
   tutorials-opencs
   tutorials-optimisation
   tutorials-chemeng-optimisation
   #tutorials-all


