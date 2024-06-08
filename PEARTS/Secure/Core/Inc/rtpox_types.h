#ifndef _RT_POX_TYPES_H
#define _RT_POX_TYPES_H

/*  RTPOX STATUS DEFINITION */
#define CLEAR_STATUS_FLAG 0x0U
#define RT_POX_SUCCESS 0U
#define RT_POX_ERROR 1U
#define RT_POX_ERROR_PERIPH_ARRAY_FULL 2U
#define RT_POX_ERROR_INVALID_PERIPH 3U
#define RT_POX_IT_ERROR 0xFFFFFFFF
#define RT_POX_ERROR_MAXIMUM_APPS_REGISTERED  4U
#define RT_POX_ERROR_APP_NAME_ALREADY_REGISTERED 5U
#define RT_POX_ERROR_MAXIMUM_INTERRUPTS 6U
#define RT_POX_ERROR_INVALID_FUNCTION_ADDRESS 7U
#define RT_POX_ERROR_PERIPH_DONT_EXIST 8U
#define RT_POX_ERROR_APP_NAME_NOT_FOUND 9U
#define RT_POX_ERROR_INVALID_STACK_SIZE 10U
#define RT_POX_ERROR_EXEC_ALREADY_RUNNING 11U


#define MAX_PERIPH 10
#define MAX_INTERRUPT 10
#define MAX_APP_SIZE 5
#define MAX_STACK_SIZE 0x1000

#define RT_POX_RUNNING_APP 0x1 
#define RT_POX_NOT_RUNNING_APP 0x2

#define FALSE 0U
#define TRUE 1U

#include <main.h>

typedef uint32_t rtpoxStatus_t;

// create a structure to hold the peripheral information
typedef struct {
    uint32_t periph;
    uint32_t sec;                   
    uint32_t priv;
} peripheral_t;

// create an struct that stores an array of peripherals
typedef struct {
    peripheral_t periph[MAX_PERIPH];
    uint32_t count;
} periph_array_t;


typedef struct{
    uint8_t exec;

}rtpox_status_t;

typedef struct {
	void * rtpox_func;           // cast to funcptr_NS
	void * rtpox_initialize;     // cast to funcptr_NS
	void * rtpox_finalize;       // cast to funcptr_NS
	void * rtpox_resume;         // cast to funcptr_NS
    uint8_t  rtpox_interrupts[MAX_INTERRUPT];
    uint8_t nInterrupts;
    uint32_t stack_size;
} rtpox_task_functions_t;

typedef struct _rtpox_reg_app{
  char appname[20];
  rtpox_task_functions_t funcs;
  uint8_t app_id;
  uint32_t psp;
  uint32_t msp;
  // todo : add peripherals
}_rtpox_reg_app_t;

typedef struct {
    _rtpox_reg_app_t app[MAX_APP_SIZE];
    uint8_t apps_count;
    uint32_t final_return_address;
    uint32_t final_ns_sp;
    uint32_t active_app;
}rtpox_app_config_t;

typedef struct {
    uint8_t id;
    uint32_t time;
    uint32_t value;
    uint32_t address;
    uint32_t syscall_id;
} rtpox_rules_t;


/* ############## ELASTIC REGION ############*/

#define MAX_ELASTIC_REGION 2

typedef struct{
    struct {
        uint32_t beg;
        uint32_t end;
    } address_space;
    uint8_t sau_region_number; 
}rtpox_elastic_region_node_t;

typedef struct{
    rtpox_elastic_region_node_t region[MAX_ELASTIC_REGION];
    uint8_t number_of_regions;
}rtpox_elastic_region_config_t;



/* ############## ATTESTATION REQUEST / REPORT  ############*/

#define CHALLANGE_SIZE  24
#define KEY_SIZE        24
typedef struct{
    uint8_t challange[CHALLANGE_SIZE];
    uint8_t app_id;
} rtpos_attestation_request_t;

typedef struct {
    uint8_t periph_violation[MAX_PERIPH];
    uint8_t memory_violation;
    uint8_t data_violation;
    uint8_t H_mem[32];
    rtpox_rules_t * rules_measurement;
    uint8_t signature;
} rtpox_report_t;

/* ############## GENERAL CONFIG ############*/

typedef struct {
    rtpox_report_t report;
    rtpox_app_config_t apps_config;
    rtpox_status_t status;

} rtpox_config_t;


/* ############## TASKS CONTEXT ############# */

#define TASK_MAXIMUM 6

typedef struct {
    struct{
        uint32_t base;
        uint32_t size;
    } memory;
    struct{
        uint32_t base;
        uint32_t size;
    } stack;
    uint32_t pc;
    uint32_t sp;
    uint32_t lr;
    uint32_t psp;
    uint32_t msp;
    uint32_t status; // 3 LSBs are used to store the status of the task
    uint8_t task_id;
}rtpox_task_t;


typedef struct {
    rtpox_task_t tasks[TASK_MAXIMUM];
    uint8_t active_task_count;
    uint32_t active_task_mask; // binary mask to store all the registerd active tasks
    uint8_t active_task_id;    // store the id of the active task that will be activated by the scheduler
    uint8_t last_task_id;     // store the id of the last task that was activated
    uint8_t sa_activate;
}rtpox_task_table_t;





typedef struct{
    uint8_t current_task;
    uint8_t next_task;
    struct{
        uint32_t val;
        uint32_t lim;
    }psp;
    struct{
        uint32_t val;
        uint32_t lim;
    }msp;

}rtpox_context_switch_t;

#endif // !_RT_POX_TYPES_H


