#ifndef __SECURE_TERMINAL__H__
#define __SECURE_TERMINAL__H__

#include "stm32l5xx_hal.h"

#define TIMEOUT 1000
#define MAX_COMMAND_LENGTH 20
#define MAX_ARG_LENGTH 20



extern UART_HandleTypeDef hlpuart1;

void st_open_terminal(void);
void st_initTerminalDebugStatus(void);

void addColor(char* input, char* output, int outputSize, char* color);

#endif  //!__SECURE_TERMINAL__H__