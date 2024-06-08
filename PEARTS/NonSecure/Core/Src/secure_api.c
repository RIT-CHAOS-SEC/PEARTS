#include <secure_nsc.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "app_freertos.h"
#include "secure_api.h"

void SM_start_attestation(void(*callback)(uint8_t*)){
    SECURE_RegisterCallback(HALT_SCHEDULER_CB_ID  , (void *)vTaskSuspendAll);
    SECURE_RegisterCallback(RESUME_SCHEDULER_CB_ID, (void *)xTaskResumeAll);
    // SECURE_RegisterCallback(ADD_TO_SCHEDULER_CB_ID, (void *)xTaskResumeAll);



}