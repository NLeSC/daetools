/***********************************************************************************
                 DAE Tools Project: www.daetools.com
                 Copyright (C) Dragan Nikolic, 2015
************************************************************************************
DAE Tools is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License version 3 as published by the Free Software
Foundation. DAE Tools is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with the
DAE Tools software; if not, see <http://www.gnu.org/licenses/>.
***********************************************************************************/
#ifndef DAE_ACTIVITY_H
#define DAE_ACTIVITY_H

#include "definitions.h"
#include "core.h"
#include "solver.h"
#include "datareporting.h"
#include "log.h"

namespace daetools
{
namespace activity
{
using namespace daetools::logging;
using namespace daetools::core;
using namespace daetools::solver;
using namespace daetools::datareporting;

enum daeeActivityAction
{
    eAAUnknown = 0,
    eRunActivity,
    ePauseActivity
};

enum daeeSimulationMode
{
    eSimulation = 0,
    eOptimization,
    eParameterEstimation
};

/*********************************************************************
    daeSimulation_t
*********************************************************************/
class daeSimulation_t
{
public:
    virtual ~daeSimulation_t(void){}

public:
    virtual daeModel_t*			GetModel(void) const                                                = 0;
    virtual void				SetModel(daeModel_t* pModel)        								= 0;
    virtual daeDataReporter_t*	GetDataReporter(void) const             							= 0;
    virtual daeLog_t*			GetLog(void) const                          						= 0;

    virtual void				SetUpParametersAndDomains(void)                 					= 0;
    virtual void				SetUpVariables(void)                                				= 0;
    virtual void				SetUpOptimization(void)                                 			= 0;
    virtual void				SetUpParameterEstimation(void)                              		= 0;
    virtual void				SetUpSensitivityAnalysis(void)                                  	= 0;
    virtual void                DoDataPartitioning(daeEquationsIndexes& equationsIndexes,
                                                   std::map<size_t,size_t>& mapVariableIndexes)     = 0;
    virtual void				DoPostProcessing(void)                                  	        = 0;

    virtual void				Run(void)                                                           = 0;
    virtual void				Finalize(void)                                          			= 0;
    virtual void				ReRun(void)                                                 	    = 0;
    virtual void				Reset(void)                                                     	= 0;
    virtual void				RegisterData(const std::string& strIteration)                       = 0;
    virtual void				ReportData(real_t dCurrentTime)                                     = 0;
    virtual void				StoreInitializationValues(const std::string& strFileName) const     = 0;
    virtual void				LoadInitializationValues(const std::string& strFileName) const      = 0;
    virtual void                SetJSONRuntimeSettings(const std::string& strJSONRuntimeSettings)   = 0;
    virtual std::string         GetJSONRuntimeSettings() const                                      = 0;
    virtual bool                GetCalculateSensitivities() const                                   = 0;
    virtual void                SetCalculateSensitivities(bool bCalculateSensitivities)             = 0;

    virtual real_t				GetCurrentTime_() const 											= 0;
    virtual void				SetTimeHorizon(real_t dTimeHorizon)									= 0;
    virtual real_t				GetTimeHorizon(void) const											= 0;
    virtual void				SetReportingInterval(real_t dReportingInterval)						= 0;
    virtual real_t				GetReportingInterval(void) const									= 0;
    virtual void				GetReportingTimes(std::vector<real_t>& darrReportingTimes) const	= 0;
    virtual void				SetReportingTimes(const std::vector<real_t>& darrReportingTimes)	= 0;
    virtual real_t				GetNextReportingTime(void) const									= 0;

    virtual void				Resume(void)														= 0;
    virtual void				Pause(void)															= 0;

    virtual daeeActivityAction	GetActivityAction(void) const										= 0;
    virtual daeeSimulationMode	GetSimulationMode(void) const										= 0;
    virtual void				SetSimulationMode(daeeSimulationMode eMode)							= 0;

    virtual void				Initialize(daeDAESolver_t* pDAESolver,
                                           daeDataReporter_t* pDataReporter,
                                           daeLog_t* pLog,
                                           bool bCalculateSensitivities = false,
                                           const std::string& strJSONRuntimeSettings = "")	= 0;
    virtual void				Reinitialize(void)                                          = 0;
    virtual void				CleanUpSetupData(void)                                      = 0;
    virtual void				SolveInitial(void)                                          = 0;
    virtual daeDAESolver_t*		GetDAESolver(void) const                                    = 0;

    virtual real_t				Integrate(daeeStopCriterion eStopCriterion,
                                          bool bReportDataAroundDiscontinuities = true)					= 0;
    virtual real_t				IntegrateForTimeInterval(real_t time_interval,
                                                         daeeStopCriterion eStopCriterion,
                                                         bool bReportDataAroundDiscontinuities = true)	= 0;
    virtual real_t				IntegrateUntilTime(real_t time,
                                                   daeeStopCriterion eStopCriterion,
                                                   bool bReportDataAroundDiscontinuities = true)		= 0;
    virtual real_t				IntegrateForOneStep(daeeStopCriterion eStopCriterion,
                                                    bool bReportDataAroundDiscontinuities = true)       = 0;

    virtual void GetOptimizationConstraints(std::vector<daeOptimizationConstraint_t*>& ptrarrConstraints) const	 = 0;
    virtual void GetOptimizationVariables(std::vector<daeOptimizationVariable_t*>& ptrarrOptVariables) const	 = 0;
    virtual void GetMeasuredVariables(std::vector<daeMeasuredVariable_t*>& ptrarrMeasuredVariables) const		 = 0;
    virtual void GetObjectiveFunctions(std::vector<daeObjectiveFunction_t*>& ptrarrObjectiveFunctions) const     = 0;
    virtual daeObjectiveFunction_t* GetObjectiveFunction(void) const	                                         = 0;

    virtual size_t GetNumberOfObjectiveFunctions(void) const													 = 0;
    virtual void   SetNumberOfObjectiveFunctions(size_t n)														 = 0;

    virtual std::map<std::string, call_stats::TimeAndCount> GetCallStats() const                                   = 0;
};

/******************************************************************
    daeActivityClassFactory_t
*******************************************************************/
class daeActivityClassFactory_t
{
public:
    virtual ~daeActivityClassFactory_t(void){}

public:
    virtual string   GetName(void) const			= 0;
    virtual string   GetDescription(void) const		= 0;
    virtual string   GetAuthorInfo(void) const		= 0;
    virtual string   GetLicenceInfo(void) const		= 0;
    virtual string   GetVersion(void) const			= 0;

    virtual daeSimulation_t*	CreateSimulation(const string& strClass)	= 0;

    virtual void SupportedSimulations(std::vector<string>& strarrClasses)	= 0;
};
typedef daeActivityClassFactory_t* (*pfnGetActivityClassFactory)(void);


}
}

#endif
