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
#include "../cs_model.h"

#if !defined(__MINGW32__) && (defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(_WIN64))
#ifdef OpenCS_SIMULATORS_EXPORTS
#define OPENCS_SIMULATORS_API __declspec(dllexport)
#else
#define OPENCS_SIMULATORS_API __declspec(dllimport)
#endif
#else
#define OPENCS_SIMULATORS_API
#endif

namespace cs
{
/* Run simulation using the input files from the specified input files directory. */
OPENCS_SIMULATORS_API void csSimulate(const std::string&                inputDirectory,
                                      std::shared_ptr<csLog_t>          log             = nullptr,
                                      std::shared_ptr<csDataReporter_t> datareporter    = nullptr,
                                      bool                              initMPI         = false);

/* Run simulation using the specified model, JSON options and simulation output directory.
 * The model must be generated by the csModelBuilder_t:PartitionSystem function.
 * Optionally, it can be loaded from input files using the csModel_t::LoadModel function. */
OPENCS_SIMULATORS_API void csSimulate(csModelPtr                        csModel,
                                      const std::string&                jsonOptions,
                                      const std::string&                simulationDirectory,
                                      std::shared_ptr<csLog_t>          log             = nullptr,
                                      std::shared_ptr<csDataReporter_t> datareporter    = nullptr,
                                      bool                              initMPI         = false);
}
