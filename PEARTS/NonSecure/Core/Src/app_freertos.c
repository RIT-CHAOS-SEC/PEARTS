/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "secure_nsc.h"
#include "app_freertos.h"



void start_secure_terminal(void){

//	__asm__("  MOVS R0, #0 \n"
//			"  MSR MSP, R0 \n");

	__ASM volatile(
	    "MOVW R0, #0xFAFA\n"  		// Load lower 16 bits
	    "MOVT R0, #0xFAFB\n"         // Load upper 16 bits
	);


  // Starts secure terminal
  SECURE_Terminal();

  // End task - should never reach here
  vTaskDelete( NULL );
}
/* USER CODE END Application */

/* USER CODE BEGIN FunctionPrototypes */

// Register secure functions --------------------------------------------------

void register_secure_scheduler_callbacks(void){
  SECURE_RegisterCallback(HALT_SCHEDULER_CB_ID  , (void *)vTaskSuspendAll);
  SECURE_RegisterCallback(RESUME_SCHEDULER_CB_ID, (void *)xTaskResumeAll);
  SECURE_RegisterCallback(ADD_TO_SCHEDULER_CB_ID, (void *)xTaskResumeAll);
}

void register_application_callbacks(void){
	SECURE_registerAttestationApp((void *)application, "matmul", 0x0 ,0xFFFFFFFF);
	return;
}

void register_secure_functions(){
  register_secure_scheduler_callbacks();
  register_application_callbacks();
}

void open_secure_terminal(void){


  SECURE_Terminal();
}


/* USER CODE END FunctionPrototypes */

