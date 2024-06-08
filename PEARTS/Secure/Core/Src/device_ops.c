/*
 * device.c
 *
 *  Created on: Feb 19, 2024
 *      Author: aj4775
 */

/* Includes ------------------------------------------------------------------*/
#include "device_ops.h"
#include "stm32l552xx.h"
#include "core_cm33.h"
#include "stm32l5xx_hal_gtzc.h"

/* inputs: none
 * outputs: none
 * description: This function initializes specific SAU region
 * */
void device_activateSAURegion( void *start, void *end, uint32_t region){
    SAU->CTRL &= ~(1UL);
    SAU->RNR = region;
    SAU->RBAR = ((uint32_t)start);
    SAU->RLAR = ((uint32_t)end | 3UL);
    SAU->CTRL |= (3UL);
    return;
}

/* inputs: none
 * outputs: none
 * description: This function deactivate specific SAU region
 * */
void device_deactivateSAURegion(uint32_t region){
    SAU->RNR = region;
    SAU->CTRL &= ~(1UL);
}

/* inputs: 
        periph : GTZC_TZSC_TZIC_PeriphId Peripheral identifier values
 * outputs: none
 * description: This function lock the peripheral for the non secure world
 * */
void device_lockPeripheral( uint32_t periph){
    HAL_GTZC_TZSC_ConfigPeriphAttributes(periph, GTZC_TZSC_PERIPH_SEC|GTZC_TZSC_PERIPH_NPRIV);
    return;
}

/* inputs:  
        periph : GTZC_TZSC_TZIC_PeriphId Peripheral identifier values
 * outputs: none
 * description: This function unlock the peripheral for the non secure world
 * */
void device_unlockPeripheral( uint32_t periph){
    HAL_GTZC_TZSC_ConfigPeriphAttributes(periph, GTZC_TZSC_PERIPH_NSEC|GTZC_TZSC_PERIPH_NPRIV);
    return;
}

/* inputs: 
        periph : GTZC_TZSC_TZIC_PeriphId Peripheral identifier values
 * outputs: none
 * description : create a function to check if a peripheral exists
 * */
uint32_t device_pheriphExists(uint32_t periph){
    if  (   ((periph >= GTZC_PERIPH_TIM2) & (periph <= GTZC_PERIPH_SPI1) ) |
            ((periph >= GTZC_PERIPH_TIM8) & (periph <= GTZC_PERIPH_EXTI) ) |
            ((periph >= GTZC_PERIPH_TZSC) & (periph <= GTZC_PERIPH_MPCBB2_REG) ) ){
        return DEVICE_SUCCESS;
    }
    return DEVICE_NOT_FOUND;
}


