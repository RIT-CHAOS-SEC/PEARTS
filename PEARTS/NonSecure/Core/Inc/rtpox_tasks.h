/*
 * rtpox_tasks.h
 *
 *  Created on: Mar 19, 2024
 *      Author: aj4775
 */

#ifndef INC_RTPOX_TASKS_H_
#define INC_RTPOX_TASKS_H_

typedef struct {
	void * rtpox_func;           // cast to funcptr_NS
	void * rtpox_initialize;     // cast to funcptr_NS
	void * rtpox_finalize;       // cast to funcptr_NS
	void * rtpox_resume;         // cast to funcptr_NS
    uint8_t  rtpox_interrupts[MAX_INTERRUPT];
    uint8_t nInterrupts;
    uint32_t stack_size;
} rtpox_task_functions_t;


#define ELASTIC_REGION __attribute__((section(".elastic_section")))
#define ELASTIC_DATA   __attribute__((section(".elastic_data")))
#define ELASTIC_REGION_NO_RETURN __attribute__((section(".elastic_section"), no_return))
#define NO_RETURN __attribute__((no_return))

void rtpox_resumeMatMul();

#endif /* INC_RTPOX_TASKS_H_ */
