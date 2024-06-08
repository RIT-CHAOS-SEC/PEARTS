/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure_nsclib/secure_nsc.h
  * @author  MCD Application Team
  * @brief   Header for secure non-secure callable APIs list
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

/* USER CODE BEGIN Non_Secure_CallLib_h */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SECURE_NSC_H
#define SECURE_NSC_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#define MAX_INTERRUPT 10
// #include <core_cm33.h>

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  non-secure callback ID enumeration definition
  */
typedef enum
{
  SECURE_FAULT_CB_ID     = 0x00U, /*!< System secure fault callback ID */
  GTZC_ERROR_CB_ID       = 0x01U,  /*!< GTZC secure error callback ID */
  HALT_SCHEDULER_CB_ID   = 0x02U,
  RESUME_SCHEDULER_CB_ID = 0x03U,
  APPLICATION_CB_ID      = 0x04U,
  ADD_TO_SCHEDULER_CB_ID = 0x05U,
} SECURE_CallbackIDTypeDef;

typedef enum
{
  SECURE_RTPOX_startAttestation, 
  SECURE_RTPOX_registerAttestationFunction,
  SECURE_RTPOX_registerInterruptHandler,
  SECURE_RTPOX_registerFreeRTOSFunction,
  SECURE_RTPOX_registerPeripheral,
} SECURE_RTPOXCallbackIDTypeDef;

typedef uint32_t SErrorStatusTypeDef; 

/* Exported constants --------------------------------------------------------*/
#define SECURE_ERROR_APP_REGISTER         0x00U /*!< System secure fault callback ID */
#define SECURE_OK                         0x01U /*!< System secure fault callback ID */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func);

void SECURE_registerAttestationApp(void *application,char *appname,uint32_t app_init_addr,uint32_t app_end_addr);

void SECURE_Terminal(void);
void SECURE_CFALOG_XA(void);


/* RTPOX FUNCTIONS */
void SECURE_rtpox_systickHandler(void);

void SECURE_rtpox_Resume();
void SECURE_rtpox_initializeFunction(char name[20],uint32_t *error);
void SECURE_rtpox_register_application(void *app_info,char name[20],uint32_t *error,uint32_t* app_id);
void tmp_SECURE_rtpox_exec_application(char name[20],uint32_t *error);
void SECURE_rtpox_endRTPoxProcess();
void SECURE_rtpox_Start(uint32_t app_id);

void SECURE_TESTEST(uint32_t a,uint32_t b,uint32_t c);


#endif /* SECURE_NSC_H */
/* USER CODE END Non_Secure_CallLib_h */

