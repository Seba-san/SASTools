/*
 * sfuntmpl_basic.c: Basic 'C' template for a level 2 S-function.
 *
 *  -------------------------------------------------------------------------
 *  | See matlabroot/simulink/src/sfuntmpl_doc.c for a more detailed template |
 *  -------------------------------------------------------------------------
 *
 * Copyright 1990-2002 The MathWorks, Inc.
 * $Revision: 1.27.4.2 $
 */


/*
 * You must specify the S_FUNCTION_NAME as the name of your S-function
 * (i.e. replace sfuntmpl_basic with the name of your S-function).
 */

#define S_FUNCTION_NAME  visualizador
#define S_FUNCTION_LEVEL 2

#include <windows.h>
#include <tchar.h>
#include <Strsafe.h>

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"
#include "VirtualDevice.h"
#include "VirtualDevice.cpp"

/*====================*
 * S-function methods *
 *====================*/

#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
  /* Function: mdlCheckParameters =============================================
   * Abstract:
   *    Validate our parameters to verify:
   *     o The numerator must be of a lower order than the denominator.
   *     o The sample time must be a real positive nonzero value.
   */
static void mdlCheckParameters(SimStruct *S)
{
    if((int)(mxGetPr(ssGetSFcnParam(S,0))[0]) > CANTIDAD_WEBCAM)
    {
        ssSetErrorStatus(S,"El n�mero m�ximo de webcams es 1");
        return;
    }
}
#endif /* MDL_CHECK_PARAMETERS */


/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    /* See sfuntmpl_doc.c for more details on the macros below */

    ssSetNumSFcnParams(S, 1);  /* Number of expected parameters */ 
    #if defined(MATLAB_MEX_FILE)
    if(ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S))
    {
        mdlCheckParameters(S);
        if(ssGetErrorStatus(S) != NULL)
            return;
    }
    else
        return; /* The Simulink engine reports a mismatch error. */
    #endif

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 3)) return;
    ssSetInputPortWidth(S, 0, 1);
	ssSetInputPortDataType(S, 0, SS_DOUBLE); 
	ssSetInputPortRequiredContiguous(S, 0, true); /*direct input signal access*/
	ssSetInputPortDirectFeedThrough(S, 0, 1); /*Set direct feedthrough flag (1=yes, 0=no).*/

	ssSetInputPortWidth(S, 1, 1);
	ssSetInputPortDataType(S, 1, SS_UINT32);
	ssSetInputPortRequiredContiguous(S, 1, true); /*direct input signal access*/
	ssSetInputPortDirectFeedThrough(S, 1, 1); /*Set direct feedthrough flag (1=yes, 0=no).*/

	ssSetInputPortWidth(S, 2, TAM_MAX);
	ssSetInputPortDataType(S, 2, SS_UINT8);
	ssSetInputPortRequiredContiguous(S, 2, true); /*direct input signal access*/
	ssSetInputPortDirectFeedThrough(S, 2, 1); /*Set direct feedthrough flag (1=yes, 0=no).*/
   
    if (!ssSetNumOutputPorts(S, 0)) return;
	    
    
    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    /* Specify the sim state compliance to be same as a built-in block */
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

    ssSetOptions(S, 0);
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0.0);

}



#define MDL_INITIALIZE_CONDITIONS   /* Change to #undef to remove function */
#if defined(MDL_INITIALIZE_CONDITIONS)
  /* Function: mdlInitializeConditions ========================================
   * Abstract:
   *    In this function, you should initialize the continuous and discrete
   *    states for your S-function block.  The initial states are placed
   *    in the state vector, ssGetContStates(S) or ssGetRealDiscStates(S).
   *    You can also perform any other initialization activities that your
   *    S-function may require. Note, this routine will be called at the
   *    start of simulation and if it is present in an enabled subsystem
   *    configured to reset states, it will be call when the enabled subsystem
   *    restarts execution to reset the states.
   */
static void mdlInitializeConditions(SimStruct *S)
{
}
#endif /* MDL_INITIALIZE_CONDITIONS */



#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START) 
  /* Function: mdlStart =======================================================
   * Abstract:
   *    This function is called once at start of model execution. If you
   *    have states that should be initialized once, this is the place
   *    to do it.
   */
static void mdlStart(SimStruct *S)
{
    int numMem;
    numMem = (int)(mxGetPr(ssGetSFcnParam(S,0))[0]) - 1;
    OpenShareMemory(M_COMPARTIDA, numMem );
    OpenControlShareMemory(M_COMPARTIDA, numMem );
}
#endif /*  MDL_START */



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    int numMem;
    UINT i;
  
    const double *u1 = (const double*) ssGetInputPortSignal(S,0);
	const UINT 	 *u2 = (const UINT*) ssGetInputPortSignal(S,1);
	const unsigned char *u3 = (const unsigned char*) ssGetInputPortSignal(S,2);
    
    static VIRTUAL_DEVICE_CONTROL pImagenControl={0};
    static VIRTUAL_DEVICE_ENTRADA pImagenEntrada={0};
    bool signalWrite;
	static bool prevSignalWrite=0;
	UINT sizeMemoryOut;
	static double u1_ant=0.0;
    
    if(*u1>u1_ant+0.01){
		if(prevSignalWrite==0) signalWrite=1;
		else signalWrite=0;
	}
	else{
		signalWrite=prevSignalWrite;
	}
	u1_ant=*u1;
    
    if(signalWrite!=prevSignalWrite){
		
		sizeMemoryOut=*u2;
		for(i=0;i<sizeMemoryOut;i++){
			pImagenEntrada.ucImagenComp[i] = u3[i];
		}
		pImagenEntrada.iCantBytesImagenComp=(int)sizeMemoryOut;
        
        WriteShareMemory(&pImagenEntrada);

		pImagenControl.signalWrite=signalWrite;
		WriteControlShareMemory(&pImagenControl);
	}

	prevSignalWrite=signalWrite;
}



#define MDL_UPDATE  /* Change to #undef to remove function */
#if defined(MDL_UPDATE)
  /* Function: mdlUpdate ======================================================
   * Abstract:
   *    This function is called once for every major integration time step.
   *    Discrete states are typically updated here, but this function is useful
   *    for performing any tasks that should only take place once per
   *    integration step.
   */
  static void mdlUpdate(SimStruct *S, int_T tid)
  {
  }
#endif /* MDL_UPDATE */



#define MDL_DERIVATIVES  /* Change to #undef to remove function */
#if defined(MDL_DERIVATIVES)
  /* Function: mdlDerivatives =================================================
   * Abstract:
   *    In this function, you compute the S-function block's derivatives.
   *    The derivatives are placed in the derivative vector, ssGetdX(S).
   */
  static void mdlDerivatives(SimStruct *S)
  {
  }
#endif /* MDL_DERIVATIVES */



/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
	CloseShareMemory();
    CloseControlShareMemory();
}


/*======================================================*
 * See sfuntmpl_doc.c for the optional S-function methods *
 *======================================================*/

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
