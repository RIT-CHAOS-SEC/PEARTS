/*
 * rtpox_tasks.c
 *
 *  Created on: Mar 18, 2024
 *      Author: aj4775
 */

#include <secure_nsc.h>
#include "cmsis_os.h"
#include "rtpox_tasks.h"
#include "application.h"
#include "stm32l5xx.h"
#include "application.h"

#define TRUE 1
#define FALSE 0

// #define USING_ACRO FALSE
#define USING_ACRO TRUE
#define TIME_MEASUREMENT TRUE

#if TIME_MEASUREMENT == TRUE
extern TIM_HandleTypeDef htim4;
#endif

#define PRESCALER_ 13

void run_application(void *argument)
{

  // #if TIME_MEASUREMENT==True
  //  init timer
  htim4.Instance->CNT = 0;
  // #endif
  for (int i = 0; i < 2000; i++)
  application();
  return;
}

ELASTIC_REGION __NO_RETURN void rtpox_main(void *argument)
{ // entry point
  // Call main function here
  run_application(argument);

  // end process
  SECURE_rtpox_endRTPoxProcess(); // end point
}

const osThreadAttr_t attapp_attributes = {
    .name = "ATT_APP",
    .priority = (osPriority_t)osPriorityAboveNormal1,
    .stack_size = 128 * 4};

ELASTIC_DATA osThreadId_t rtpox_matmulTaskHandle;

ELASTIC_REGION void rtpox_finalize()
{
  vTaskDelete(NULL);
}
//
#pragma GCC push_options
#pragma GCC optimize("-O3")
void *_rtpox_resume;
ELASTIC_REGION NO_RETURN void rtpox_resume()
{
  /* Do not change the content of this function */
  while (*_ICSR_NS & (1 << 26))
  {
  }
  ((void (*)())_rtpox_resume)();
}
#pragma GCC pop_options

/* declare the .. function */
ELASTIC_REGION void rtpox_runRTPOX()
{

  /* Do not change the content of this function */
  // load 0x1000000 to r0
  __asm volatile("mov r0, #0x1000000\n\t");
  // __asm volatile("mrs r0, PSR\n\t");
  __asm volatile("push {r0}\n\t"
                 "push {r0}\n\t");
  // add pc to stack
  __asm volatile("mov r0, pc\n\t"
                 "push {r0}\n\t");
  // add rs to stack
  __asm volatile("push {r12}\n\t"
                 "push {r3}\n\t"
                 "push {r2}\n\t"
                 "push {r1}\n\t"
                 "push {r0}\n\t");
  // call the secure world to start rtpox
  SECURE_rtpox_Start(app_id);
}

/* declare the function dispatcher*/
ELASTIC_REGION void rtpox_initialize()
{
  rtpox_matmulTaskHandle = osThreadNew(rtpox_runRTPOX, NULL, &attapp_attributes);
}

ELASTIC_REGION void rtpox_register()
{
  uint32_t errorStatus;
  rtpox_task_functions_t tf;
  SECURE_rtpox_Resume(&_rtpox_resume); // get resume function trigger address
  tf.rtpox_resume = (void *)rtpox_resume;
  tf.rtpox_func = (void *)rtpox_main;
  tf.rtpox_finalize = (void *)rtpox_finalize;
  tf.rtpox_initialize = (void *)rtpox_runRTPOX; // will not be used
  tf.nInterrupts = 0U;
  tf.stack_size = 4 * 128;
  SECURE_rtpox_register_application(&tf, "APP_TASK", &errorStatus, &app_id);
}

ELASTIC_REGION void rtpox_startMatMul()
{
#if TIME_MEASUREMENT == TRUE
  //	 init timer

htim4.Init.AutoReloadPreload = 800;
  htim4.Init.Prescaler = 1 << PRESCALER_;
  HAL_TIM_Base_Init(&htim4);
  HAL_TIM_Base_Start_IT(&htim4);

  HAL_DBGMCU_EnableDBGStandbyMode();
  HAL_DBGMCU_EnableDBGStopMode();
  DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_TIM4_STOP;

//  __HAL_RCC_TIM4_CLK_ENABLE();
//  NVIC_EnableIRQ(TIM4_IRQn);


  HAL_SYSTICK_Config(SystemCoreClock / (1000U * (uint32_t)FREQ));
#endif

#if USING_ACRO == TRUE
  rtpox_register();
  rtpox_initialize();
#else
  rtpox_matmulTaskHandle = osThreadNew(run_application, NULL, &attapp_attributes);

#endif
}

//13486 12178 ....8192
/* **************************************************/
