/*
 * rtpox.c
 *
 *  Created on: Feb 19, 2024
 *      Author: aj4775
 */

// Include the header file
#include "rtpox.h"
#include "device_ops.h"
#include "stm32l552xx.h"
#include "string.h"
#include "rtpox_types.h"
#include "rtpox_its.h"
#include <main.h>
periph_array_t  periph_array;
rtpox_config_t  rtpox_config;
rtpox_task_table_t rtpox_task_table;

/* ********************************** REGISTERING PERIPHERALS ********************************** */
/* inputs: none
 * outputs: none
 * description: Create a function to initialize a peripheral array
* */
void  _rtpox_createPeripheralArray(){
    periph_array.count = 0;
    return ;
}

/* inputs: none
 * outputs: none
 * description: Create a function to check if a peripheral exists
* */
rtpoxStatus_t _rtpox_pheriphExists(uint32_t periph){
    if (device_pheriphExists(periph) == RT_POX_SUCCESS) return RT_POX_SUCCESS;
    else return RT_POX_ERROR_INVALID_PERIPH;
}

//#region Deletion

/* inputs: none
 * outputs: none
 * description: Create a generic function to register a peripheral
* */
rtpoxStatus_t _rtpox_registerPeripheral(uint32_t periph,periph_array_t periph_array){
    rtpoxStatus_t stat;
    if (periph_array.count >= MAX_PERIPH){
        return RT_POX_ERROR_PERIPH_ARRAY_FULL;
    }

    stat = _rtpox_pheriphExists(periph);
    if (stat != RT_POX_SUCCESS){
        return stat;
    }

    periph_array.periph[periph_array.count].periph = periph;
    periph_array.periph[periph_array.count].sec = 0;
    periph_array.periph[periph_array.count].priv = 0;
    periph_array.count++;
    return RT_POX_SUCCESS;
}

/* inputs: none
 * outputs: none
 * description: This function register the peripherals in the peripherals array
* */
void rtpox_registerPeripheral(uint32_t periph){
    _rtpox_registerPeripheral(periph,periph_array);
}

/* inputs: none
 * outputs: none
 * description: This function initializes the peripherals array manually
* */
void rtpox_manualInitialzePeriph(){
    // rtpox_registerPeripheral(GTZC_PERIPH_USART1,app_ppa);
    return;
}

/* inputs: none
 * outputs: none
 * description: This function locks all the peripherals
* */
void rtpox_lockAllPeripheral(){
    for (int i = 0; i < periph_array.count; i++){
        device_lockPeripheral(periph_array.periph[i].periph);
    }
    return;
}

/* inputs: none
 * outputs: none
 * description: This function unlocks all the peripherals
* */
void rtpox_unlockAllPeripheral(periph_array_t * app_ppa){
    for (int i = 0; i < app_ppa->count; i++){
        device_unlockPeripheral(app_ppa->periph[i].periph);
    }
    return;
}

/* ********************************** REGISTRATION OF APP ********************************** */
#define NOT_IN_RANGE(x,min,max) (((x<min)||(x>max))?1:0)

rtpoxStatus_t rtpox_config_registerNSTasks(rtpox_task_functions_t* funcs,char name[20],int32_t* _app_id){
    // check if reached maximum apps
    if( rtpox_config.apps_config.apps_count >= MAX_APP_SIZE)
        return RT_POX_ERROR_MAXIMUM_APPS_REGISTERED;
    // check if app name is already registered
    for(int i=0; i<rtpox_config.apps_config.apps_count;i++){
        if(strcmp(rtpox_config.apps_config.app[i].appname,name) == 0)
            return RT_POX_ERROR_APP_NAME_ALREADY_REGISTERED;
    }

    uint8_t app_id = rtpox_config.apps_config.apps_count;

    // copy stack size
    rtpox_config.apps_config.app[app_id].funcs.stack_size = funcs->stack_size;

    // check if stack size is valid
    if(funcs->stack_size > MAX_STACK_SIZE & funcs->stack_size % 8 != 0)
        return RT_POX_ERROR_INVALID_STACK_SIZE;

    // verify if nInterrupts is less than MAX_INTERRUPT
    if(funcs->nInterrupts > MAX_INTERRUPT)
        return RT_POX_ERROR_MAXIMUM_INTERRUPTS;

    // test if peripheral is available
    for (int i = 0; i < funcs->nInterrupts; i++){
        if (funcs->rtpox_interrupts[i] > 15 & funcs->rtpox_interrupts[i] < 107){
            return RT_POX_ERROR_PERIPH_DONT_EXIST;
        }
    }

    // verify if func addresses are in the elastic region
    if( NOT_IN_RANGE(funcs->rtpox_initialize,0x08070000U,0x08080000U) ||
		NOT_IN_RANGE(funcs->rtpox_func,0x08070000U,0x08080000U) ||
		NOT_IN_RANGE(funcs->rtpox_resume,0x08070000U,0x08080000U) ||
		NOT_IN_RANGE(funcs->rtpox_finalize,0x08070000U,0x08080000U))
    				return RT_POX_ERROR_INVALID_FUNCTION_ADDRESS;

    // todo: test interrupt handler ranges

    // register the app
    strcpy(rtpox_config.apps_config.app[app_id].appname,name);
    // register the functions
    memcpy(&rtpox_config.apps_config.app[app_id].funcs,funcs,sizeof(rtpox_task_functions_t));
    // increment the app count
    rtpox_config.apps_config.apps_count++;

    *_app_id = (uint32_t)app_id;

    return RT_POX_SUCCESS;
}

/* ********************************** APP OPERATIONS  ********************************** */
rtpoxStatus_t rtpox_getAppId(char name[20],uint8_t * app_id){
    for(int i=0; i<rtpox_config.apps_config.apps_count;i++){
        if(strcmp(rtpox_config.apps_config.app[i].appname,name) == 0){
            *app_id = i;
            return RT_POX_SUCCESS;
        }
    }
    return RT_POX_ERROR_APP_NAME_NOT_FOUND;
}

rtpoxStatus_t rtpox_execAppID(uint32_t app_id){
    // check if app_id is valid
    if(app_id >= rtpox_config.apps_config.apps_count)
        return RT_POX_ERROR_APP_NAME_NOT_FOUND;
    // Get the app main function
    funcptr_NS appMain = (funcptr_NS)rtpox_config.apps_config.app[app_id].funcs.rtpox_func;
    // execute the app
    appMain();
    return RT_POX_SUCCESS;
}

rtpoxStatus_t rtpox_getAppAddress(uint32_t app_id, uint32_t* address){
    // check if app_id is valid
    if(app_id >= rtpox_config.apps_config.apps_count)
        return RT_POX_ERROR_APP_NAME_NOT_FOUND;
    // Get the app main function
    *address = (funcptr_NS)rtpox_config.apps_config.app[app_id].funcs.rtpox_func;

    return RT_POX_SUCCESS;
}

/* ********************************** FINALIZATION ROUTINE *******************************/
void __attribute__((naked)) rtpox_endRTPoxProcess(){
        rtpox_its_endContext();
        // load ns_sps
        __ASM volatile("msr psp_ns, %0\n\t"::"r"(rtpox_config.apps_config.final_ns_sp));
        // mov returning address to lr
        __ASM volatile("mov lr, %0\n\t"::"r"(rtpox_config.apps_config.app[rtpox_config.apps_config.active_app].funcs.rtpox_finalize));
        // bxns to returning address
        __ASM volatile(
            "mov r0,lr\n\t"
            "movs r1, #1\n\t"
            "bics r0, r1\n\t"
            "bxns r0\n\t");
}

/* ********************************** INITIALIZATION ROUTINE *******************************/

extern _rtpox_elastic_ram_start;
uint32_t aux;
void __attribute__((naked)) rtpox_startRTPoxProcess(){

	// move r0 to app_id
	__ASM volatile("mov %0,r4 \n\t"::"r"(rtpox_config.apps_config.active_app));
    // move r1 to rtpox_config.apps_config.final_return_address
    __ASM volatile("mov %0, r5\n\t":"=r"(rtpox_config.apps_config.final_return_address));
    // store r2 to rtpox_config.apps_config.final_ns_sp address
    __ASM volatile("mov %0, r6\n\t":"=r"(rtpox_config.apps_config.final_ns_sp));
    // set ns_psp register as &_rtpox_elastic_ram_start
    __ASM volatile("msr psp_ns, %0\n\t"::"r"(&_rtpox_elastic_ram_start));
    // clear last bit of the return pointer
    aux = (((uint32_t)rtpox_config.apps_config.app[rtpox_config.apps_config.active_app].funcs.rtpox_func) & ~(0x1U));

    // set the active app 
    rtpox_config.apps_config.active_app = 0; // todo, change if more apps are added
    // load  (uint32_t)rtpox_config.apps_config.app[app_id].funcs.rtpox_func) & ~(0x1U) to lr
    __ASM volatile("mov lr, %0\n\t"::"r"( aux));
    // bxns to the function
    __ASM volatile("bxns lr\n\t");
}

/* ********************************** INIT ********************************** */

rtpoxStatus_t rtpox_initializeRTPOX(uint32_t app_id, uint32_t exc_return){
    // check if a RTPOX process is already running
    if(rtpox_config.status.exec == TRUE)
        return RT_POX_ERROR_EXEC_ALREADY_RUNNING;
    rtpox_config.status.exec = TRUE;

    // allocate stack
    // sp = rtpox_memory_allocateStack(app_id);
    uint32_t sp = 0x2003E000U; //todo
    rtpox_task_table.tasks[app_id].stack.base = sp;
    rtpox_task_table.tasks[app_id].stack.size = rtpox_config.apps_config.app[app_id].funcs.stack_size;
    rtpox_task_table.tasks[app_id].sp = sp;
    rtpox_task_table.tasks[app_id].pc = (uint32_t)rtpox_config.apps_config.app[app_id].funcs.rtpox_func;

    // initialize the peripherals
    // todo

    // initialize the interrupts
    rtpox_task_table.active_task_count ++;

    // set the active task
    rtpox_task_table.last_task_id = rtpox_task_table.active_task_id;
    rtpox_task_table.active_task_id = app_id;
    // activate PENDSV interrupt
    SET_BIT(SCB->ICSR,28);

    return RT_POX_SUCCESS;
}


rtpoxStatus_t rtpox_finalizeRTPOX(uint8_t app_id){

    // reduce active tasks counter
    rtpox_task_table.active_task_count--;
    
    //call the finalize function
    funcptr_NS appFinalize = (funcptr_NS)rtpox_config.apps_config.app[app_id].funcs.rtpox_finalize;
    appFinalize();
}

/* inputs: none
 * outputs: none
 * description: This function initializes the rtpox variables
*/
void rtpox_init(){
    _rtpox_createPeripheralArray();
    // rtpox_manualInitialzePeriph(app_periph_array);
}

/* ********************************** DUMB DEBUGGER ********************************** */
void rtpox_cleanStructs(){
    rtpox_config.apps_config.apps_count = 0;
    rtpox_config.status.exec = FALSE;
    rtpox_task_table.active_task_count = 0;
    rtpox_task_table.active_task_id = -1;
    rtpox_task_table.last_task_id = -1;
    rtpox_task_table.sa_activate = FALSE;
}

void rtpox_deb_donothing(){
    rtpox_cleanStructs();
}


