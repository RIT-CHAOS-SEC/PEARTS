#ifndef RT_POX_H
#define RT_POX_H 

#include"rtpox_types.h"

void rtpox_deb_donothing();
rtpoxStatus_t rtpox_execAppID(uint32_t app_id);
rtpoxStatus_t rtpox_getAppId(char name[20],uint8_t * app_id);
rtpoxStatus_t rtpox_getAppAddress(uint32_t app_id, uint32_t* address);
rtpoxStatus_t rtpox_config_registerNSTasks(rtpox_task_functions_t* funcs,char name[20],int32_t* _app_id);


/* FINALIZATION ROUTINE*/
void __attribute__((naked)) rtpox_endRTPoxProcess();
/*INITIALIZATION ROUTINE*/
void __attribute__((naked)) rtpox_startRTPoxProcess();
#endif // !1

