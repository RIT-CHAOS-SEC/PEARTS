/**
* @file:	app_freertos.h
* @author:	
* @date:	2023年11月06日 12:02:43 Monday
* @brief:	
**/

#ifndef __APP_FREERTOS__H__
#define __APP_FREERTOS__H__

void __attribute__((section(".att_funcs"))) application(void);
void __attribute__((section(".att_funcs"))) application2(void);
void start_secure_terminal();


void register_secure_functions();


#endif  //!__APP_FREERTOS__H__




