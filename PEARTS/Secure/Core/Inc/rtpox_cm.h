#ifndef _RT_POX_CS_H

#include <main.h>   




/* rtpox_task_table_node_t FLAGS */
#define TASK_STATUS_ACTIVE_MASK         0x0
#define TASK_STATUS_TERMINATED_MASK     0x1
#define TASK_STATUS_SUSPENDED_MASK      0x2
#define TASK_STATUS_ERROR_MASK          0x3

#define SET_BIT(REG,MASK)              ((REG) |= (0x1U << MASK))
#define CLEAR_BIT(REG,MASK)            ((REG) &= ~(0x1U << MASK))
#define READ_BIT(REG,MASK)             (((REG) & (0x1U << MASK))>>MASK)
#define TOGGLE_BIT(REG,MASK)           ((REG) ^= (0x1U << MASK))

#define SET_TASK_STATUS(REG,STATUS)    (REG = ((REG & ~(0b111U)) | (STATUS)))
#define GET_TASK_STATUS(REG)           (REG & 0b111U)

#define STM32_NUMBER_OF_PERIPHERALS     107
#define STM32_NUMBER_OF_PERIPHERALS_32b (107/32)+1
#define MAX_TASKS 6

// typedef struct {
//     struct{
//         uint32_t base;
//         uint32_t size;
//     } memory;
//     struct{
//         uint32_t base;
//         uint32_t size;
//     } stack;
//     struct {
//         uint32_t sp, lr, pc, r0, r1, r2, r3, r4, r5, r6, r7;
//         uint32_t r8, r9, r10, r11, r12, xpsr, msp, psp;
//     } registers;
//     uint32_t status; // 3 LSBs are used to store the status of the task
//     uint8_t task_id;
// } rtpox_task_table2_t;

// typedef struct {
//     uint32_t status;
//     uint32_t active_task;
//     uint32_t error;
//     uint8_t task_count;
//     struct{
//         uint32_t base;
//         uint32_t size;
//     } elastic_region;
//     struct{
//         uint32_t base;
//         uint32_t size;
//     } available_memory;
//     struct{
//         uint32_t base;
//         uint32_t size;
//     } stack_memory;
//     uint32_t rtpox_peripherals[STM32_NUMBER_OF_PERIPHERALS_32b];
//     rtpox_task_table2_t task_table[MAX_TASKS];
// } rtpox_cm_state_t;


#endif // !_RT_POX_CS_H

