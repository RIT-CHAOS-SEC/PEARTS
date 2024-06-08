/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    Secure/Src/secure_nsc.c
 * @author  MCD Application Team
 * @brief   This file contains the non-secure callable APIs (secure world)
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
// #include "secure_terminal.h"
#include <string.h>
#include <cfa.h>
#include <rtpox_types.h>
#include <rtpox.h>
#include <rtpox_its.h>
/** @addtogroup STM32L5xx_HAL_Examples
 * @{
 */

/** @addtogroup Templates
 * @{
 */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL; /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL; /* Pointer to secure error callback in Non-secure */
void *pSecureHaltSchedulerCallback = NULL;
void *pSecureResumeSchedulerCallback = NULL;
void *pSecureApplicationCallback[] = {NULL, NULL, NULL, NULL, NULL};
char pSecureApplicationNameList[4][20];
int maxApplication = sizeof(pSecureApplicationNameList) / sizeof(pSecureApplicationNameList[0]);
int currentApplication = 0;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Secure registration of non-secure callback.
 * @param  CallbackId  callback identifier
 * @param  func        pointer to non-secure function
 * @retval None
 */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
    if (func != NULL)
    {
        switch (CallbackId)
        {
        case SECURE_FAULT_CB_ID: /* SecureFault Interrupt occurred */
            pSecureFaultCallback = func;
            break;
        case GTZC_ERROR_CB_ID: /* GTZC Interrupt occurred */
            pSecureErrorCallback = func;
            break;
        default:
            /* unknown */
            break;
        }
    }
}

CMSE_NS_ENTRY SErrorStatusTypeDef SECURE_RegisterApplication(SECURE_CallbackIDTypeDef CallbackId, void *func, char *funcName)
{
    if (func != NULL && currentApplication < maxApplication && CallbackId == APPLICATION_CB_ID)
    {
        /* SecureFault Interrupt occurred */
        pSecureApplicationCallback[currentApplication] = func;
        strcpy(pSecureApplicationNameList[currentApplication], funcName);
        currentApplication++;
        return SECURE_OK;
    }
    return SECURE_ERROR_APP_REGISTER;
}

/**
 * @brief  Register the new applications.
// * @retval None
// */
CMSE_NS_ENTRY void SECURE_registerAttestationApp(void *application,
                                                char *appname,
                                                uint32_t app_init_addr,
                                                uint32_t app_end_addr)
{

   if (cfa_register_application((funcptr_NS)application, appname, app_init_addr, app_end_addr) != SUCCESS)
   {
       Error_Handler();
   }

   return;
}

/**
 * @brief  Open Secure terminal to serial communication.
 * @retval None -> Should never return
 */
CMSE_NS_ENTRY void SECURE_Terminal(void)
{
    st_open_terminal();
}

/*  #####################  RTPOX  #####################  */

/**
 * @brief  .
 * @retval None
 */
CMSE_NS_ENTRY void SECURE_rtpox_secureCall(SECURE_RTPOXCallbackIDTypeDef callbackId,
                                           void *arg)
{
    switch (callbackId)
    {
    case SECURE_RTPOX_startAttestation:
//        rtpox_cm_startAttestation((rtpos_attestation_request_type*)arg);
        break;
    case SECURE_RTPOX_registerAttestationFunction:
//        _rtpox_registerAttestationFunction(arg, (_rtpox_reg_app_t *)arg);
        break;
    case SECURE_RTPOX_registerInterruptHandler:
        // todo
    case SECURE_RTPOX_registerFreeRTOSFunction:
        // todo
    default:
        break;
    }
    return;
}


/* **************************** RTPOX APIs ***************************** */
#include <rtpox_its.h>

uint32_t errorFlag = 0;
#define INVALID_FUNCTION_ADDRESS 0x1U
typedef struct {
	void * rtpox_func;
	void * rtpox_initialize;
	void * rtpox_finalize;
	void * rtpox_resume;
    void * rtpox_interrupts;
    uint32_t nInterrupts;
} rtpox_task_funtions_t;

CMSE_NS_ENTRY void SECURE_rtpox_systickHandler(void)
{
//    rtpox_interruptDispatcher_SystickNS();
}

CMSE_NS_ENTRY void SECURE_rtpox_Resume(uint32_t* func)
{
	*func = rtpox_it_getResume();
    return;
}

CMSE_NS_ENTRY void SECURE_rtpox_initializeFunction(char name[20],uint32_t *error)
{

    uint8_t app_id = 0;
    rtpoxStatus_t * status = (rtpoxStatus_t*) error;
    *status = CLEAR_STATUS_FLAG;

    *status = rtpox_getAppId(name, &app_id);
    if(*status != RT_POX_SUCCESS)
        return;

    // initialize interrupts 
    // rtpox_initITS();

    // initialize memory
    // todo

    // initialize peripherals
    // todo

    // initialize task
    *status = rtpox_execAppID(app_id);
    if(*status != RT_POX_SUCCESS)
        return;
}


CMSE_NS_ENTRY void SECURE_rtpox_initializeRTPOX (char name[20],uint32_t *error)
{

    uint8_t app_id;
    rtpoxStatus_t * status = (rtpoxStatus_t*) error;
    *status = CLEAR_STATUS_FLAG;

    *status = rtpox_getAppId(name, &app_id);
    if(*status != RT_POX_SUCCESS)
        return;

    
    // mov app_id to r0
    
    // __ASM volatile("mov r0, %0"::"r"(app_id));
    // // load lr to r1
    // __ASM volatile("mov r1, lr");
    // // b rtpox_initializeRTPOX
    // __ASM volatile("b rtpox_initializeRTPOX");

    *status = rtpox_execAppID(app_id);


}
uint32_t returning_address;
uint32_t ns_psp;
uint8_t app_id;
uint32_t func_address;

CMSE_NS_ENTRY __attribute__((naked)) void SECURE_rtpox_Start(uint32_t app_id){


    // push r4 to r6
    __ASM volatile("push {r4-r6}\n\t");
    
    // move app_id to r0
    __ASM volatile("mov r4, %0\n\t"::"r"(app_id));

    // __ASM volatile("mov %0, lr\n\t":"=r"(returning_address));
    __ASM volatile("mov r5, lr\n\t");
    // save ns stack pointer
    // __ASM volatile("mrs %0, psp_ns\n\t":"=r"(ns_psp));
    __ASM volatile("mrs r6, psp_ns\n\t");


    rtpox_its_setAllInterruptsAsDumb();


    // call the rtpox_startRTPoxProcess
    __ASM volatile("b rtpox_startRTPoxProcess\n\t");
    

    // // get function address
    // rtpox_getAppAddress(app_id,&func_address);
    // // bxns func_address
    // __ASM volatile("bxns %0\n\t"::"r"( (func_address & ~(0x1U))));
}

CMSE_NS_ENTRY __attribute__((naked)) void SECURE_rtpox_endRTPoxProcess(){
    // // load ns_sps
    // __ASM volatile("msr psp_ns, %0\n\t"::"r"(ns_psp));
    // // mov returning address to lr
    // __ASM volatile("mov lr, %0\n\t"::"r"(returning_address));
    // // bxns to returning address
    // __ASM volatile("bxns lr\n\t");

    // b to rtpox_endRTPoxProcess
    __ASM volatile("b rtpox_endRTPoxProcess\n\t");
}

CMSE_NS_ENTRY void tmp_SECURE_rtpox_exec_application(char name[20],uint32_t *error)
{
    uint8_t app_id;
    rtpoxStatus_t * status = (rtpoxStatus_t*) error;
    *status = CLEAR_STATUS_FLAG;

    *status = rtpox_getAppId(name, &app_id);
    if(*status != RT_POX_SUCCESS)
        return;

    *status = rtpox_execAppID(app_id);
    if(*status != RT_POX_SUCCESS)
        return;
}

CMSE_NS_ENTRY void SECURE_rtpox_register_application (void *app_info,char name[20],uint32_t *error,uint32_t * app_id)
{
   *error = rtpox_config_registerNSTasks((rtpox_task_functions_t*) app_info, name,app_id);
   return;
}


uint32_t a_,b_,c_;
CMSE_NS_ENTRY void SECURE_TESTEST(uint32_t a,uint32_t b,uint32_t c){
    a_ = a;
    b_ = b;
    c_ = c;
}

/* USER CODE END Non_Secure_CallLib */
