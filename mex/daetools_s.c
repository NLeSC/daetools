#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME  daetools_s

#include "simstruc.h"
#include "daetools_matlab_common.h"

#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS)  && defined(MATLAB_MEX_FILE)
static void mdlCheckParameters(SimStruct *S)
{
    const mxArray *pPythonPath          = ssGetSFcnParam(S,0);
    const mxArray *pSimulationCallable  = ssGetSFcnParam(S,1);
    const mxArray *pCallableArguments   = ssGetSFcnParam(S,2);
    const mxArray *pNumberOfInletPorts  = ssGetSFcnParam(S,3);
    const mxArray *pNumberOfOutletPorts = ssGetSFcnParam(S,4);

    if(!IS_PARAM_STRING(pPythonPath))
    {
        ssSetErrorStatus(S, "First parameter must be a string (full path to the python file)");
        return;
    }

    if(!IS_PARAM_STRING(pSimulationCallable))
    {
        ssSetErrorStatus(S, "Second argument must be a string (name of python callable object)");
        return;
    }

    if(!IS_PARAM_STRING_EMPTY_ALLOWED(pCallableArguments))
    {
        ssSetErrorStatus(S, "Third argument must be a string (callable object arguments)");
        return;
    }

    if(!IS_PARAM_UINT(pNumberOfInletPorts))
    {
        ssSetErrorStatus(S, "Fourth parameter must be an unsigned integer (number of inlet ports)");
        return;
    }

    if(!IS_PARAM_UINT(pNumberOfOutletPorts))
    {
        ssSetErrorStatus(S, "Fifth parameter must be an unsigned integer (number of outlet ports)");
        return;
    }
}
#endif


static void mdlInitializeSizes(SimStruct *S)
{
    int i;
    ssSetNumSFcnParams(S, 5);

#if defined(MATLAB_MEX_FILE)
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S))
    {
        mdlCheckParameters(S);
        if(ssGetErrorStatus(S) != NULL)
            return;
    }
    else
    {
        return;
    }
#endif

    ssSetSFcnParamTunable(S, 0, false);
    ssSetSFcnParamTunable(S, 1, false);
    ssSetSFcnParamTunable(S, 2, false);
    ssSetSFcnParamTunable(S, 3, false);
    ssSetSFcnParamTunable(S, 4, false);

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    /* Set the number of input ports to the third parameter.
     * Ports' width will be inherited from the connected ports. */
    unsigned int nInletPorts = *(mxGetPr(ssGetSFcnParam(S, 3)));
    if (!ssSetNumInputPorts(S, nInletPorts))
        return;

    for(i = 0; i < nInletPorts; i++)
    {
        ssSetInputPortWidth(S, i, DYNAMICALLY_SIZED);
        ssSetInputPortRequiredContiguous(S, i, true);
        ssSetInputPortDataType(S, i, SS_DOUBLE);
        ssSetInputPortDirectFeedThrough(S, i, true);
    }

    /* Set the number of output ports to the fourth parameter.
     * Ports' width must be 1 at the moment. */
    unsigned int nOutletPorts = *(mxGetPr(ssGetSFcnParam(S, 4)));
    if (!ssSetNumOutputPorts(S, nOutletPorts))
        return;

    for(i = 0; i < nOutletPorts; i++)
    {
        ssSetOutputPortWidth(S, i, 1);
        ssSetOutputPortDataType(S, i, SS_DOUBLE);
    }

    ssSetNumSampleTimes(S, 1);

    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 1);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    ssSetOptions(S, 0);
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_START
static void mdlStart(SimStruct *S)
{
    if(debugMode)
        ssPrintf("Loading the simulation...\n");

    char* pyFile            = mxArrayToString(ssGetSFcnParam(S, 0));
    char* simCallableName   = mxArrayToString(ssGetSFcnParam(S, 1));
    char* simArguments      = mxArrayToString(ssGetSFcnParam(S, 2));

    if(debugMode)
    {
        ssPrintf("Python file: %s\n",               pyFile);
        ssPrintf("Python callable name: %s\n",      simCallableName);
        ssPrintf("Python callable arguments: %s\n", simArguments);
    }

    /* Load the simulation object from the specified file using 
     * the callable object and its arguments. */
    void *simulation = LoadSimulation(pyFile, simCallableName, simArguments);
    if(!simulation)
    {
        ssSetErrorStatus(S, "Cannot load DAETools simulation");
        return;
    }

    /* Save the simulation object for later use. */
    ssGetPWork(S)[0] = simulation;

    /* Initialize the simulation with the default settings */
    if(debugMode)
        ssPrintf("Initializing simulation...\n");

    /* Free memory */
    mxFree(pyFile);
    mxFree(simCallableName);
    mxFree(simArguments);

    /* Get the number of parameters, inlet and outlet ports and check their number */
    int i;
    int nParameters  = GetNumberOfParameters(simulation);
    int nInletPorts  = GetNumberOfInputs(simulation);
    int nOutletPorts = GetNumberOfOutputs(simulation);

    if(debugMode)
    {
        ssPrintf("Number of parameters: %d\n", nParameters);
        ssPrintf("Number of inputs: %d\n",     nInletPorts);
        ssPrintf("Number of outputs: %d\n",    nOutletPorts);
    }

    if(ssGetNumInputPorts(S) != nInletPorts)
    {
        sprintf(msg, "Invalid number of input ports: %d (should be %d)", ssGetNumInputPorts(S), nInletPorts);
        ssSetErrorStatus(S, msg);
        return;
    }

    if(ssGetNumOutputPorts(S) != nOutletPorts)
    {
        sprintf(msg, "Invalid number of output ports: %d (should be %d)", ssGetNumOutputPorts(S), nOutletPorts);
        ssSetErrorStatus(S, msg);
        return;
    }

    /* Set a dummy time horizon and reporting interval. */
    SetReportingInterval(simulation, 1.0);
    SetTimeHorizon(simulation,       10.0);
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
    /* Get the simulation object from the PWork array */
    void *simulation = (void *) ssGetPWork(S)[0];

    /* Set the time horizon (for the current step only)
     * Achtung, Achtung!!
     *   Try to obtain an overall time horizon (final time of the simulation) */
    time_t stepStartTime   = ssGetTStart(S);
    time_T stepTimeHorizon = ssGetT(S);

    int i;
    unsigned int noPoints;
    char name[512];

    int nInletPorts  = GetNumberOfInputs(simulation);
    int nOutletPorts = GetNumberOfOutputs(simulation);

    if(stepTimeHorizon == 0)
    {
        /* Solve the system with the specified initial conditions and report data. */
        if(debugMode)
            ssPrintf("Solving initial system at time %.6f ...\n", stepTimeHorizon);
        SolveInitial(simulation);
        ReportData(simulation);
    }
    else
    {
        /* Integrate until the specified step final time.
         * Achtung, Achtung!!
         *   How to set the reporting interval during the initialization phase?
         *   Do we need to set it in advance, or just report the data after every mdlOuptuts call? */
        if(debugMode)
            ssPrintf("Integrating from %.6f to %.6f ...\n", stepStartTime, stepTimeHorizon);

        /* Set the time horizon (see if it can be done before the simulation start). */
        SetTimeHorizon(simulation, stepTimeHorizon);

        /* Set the inlet ports' values. */
        for(i = 0; i < nInletPorts; i++)
        {
            GetInputInfo(simulation, i, name, &noPoints);
            if(debugMode)
                ssPrintf("Input %d name: %s, no.points: %d\n", i, name, noPoints);
            if(noPoints != ssGetInputPortWidth(S, i))
            {
                sprintf(msg, "Invalid width of port %s: %d (expected %d)", name, ssGetInputPortWidth(S, i), noPoints);
                ssSetErrorStatus(S, msg);
                return;
            }
            const double* data = ssGetInputPortRealSignal(S, i);
            if(debugMode)
                ssPrintf("Set input %d value to: %f\n", i, data[0]);
            SetInputValue(simulation, i, data, noPoints);
        }

        /* Integrate until specified time and report data. */
        Reinitialize(simulation);
        IntegrateUntilTime(simulation, stepTimeHorizon);
        ReportData(simulation);
    }

    /* Set the outputs' values */
    for(i = 0; i < nOutletPorts; i++)
    {
        /* Check the oulet port width. */
        GetOutputInfo(simulation, i, name, &noPoints);
        if(debugMode)
            ssPrintf("Output %d name: %s, no.points: %d\n", i, name, noPoints);
        if(noPoints != ssGetOutputPortWidth(S, i))
        {
            sprintf(msg, "Invalid width of outlet port %s: %d (expected %d)", name, ssGetOutputPortWidth(S, i), noPoints);
            ssSetErrorStatus(S, msg);
            return;
        }

        /* Get the result from daetools. */
        double* data = (double*)malloc(noPoints * sizeof(double));
        GetOutputValue(simulation, i, data, noPoints);
        if(debugMode)
            ssPrintf("Get output %d value: %f\n", i, data[0]);

        /* Set the outlet port value. */
        double* y = ssGetOutputPortRealSignal(S, i);
        memcpy(y, data, noPoints * sizeof(double));

        /* Free the data buffer. */
        free(data);
    }
}

static void mdlTerminate(SimStruct *S)
{
    /* Get the simulation object from the PWork array */
    void *simulation = (void *) ssGetPWork(S)[0];

    /* Finalize the simulation and free the object */
    Finalize(simulation);
    FreeSimulation(simulation);
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

