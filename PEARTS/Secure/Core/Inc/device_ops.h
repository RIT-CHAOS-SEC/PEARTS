
#ifndef DEVICE_OPS_H
#define DEVICE_OPS_H

#define DEVICE_SUCCESS 0
#define DEVICE_FAILURE 1 
#define DEVICE_NOT_FOUND 2

#include <main.h>

// Peripherals Configuration
uint32_t device_pheriphExists(uint32_t periph);
void device_lockPeripheral( uint32_t periph);
void device_unlockPeripheral( uint32_t periph);

// SAU Configuration
void device_deactivateSAURegion(uint32_t region);
void device_activateSAURegion( void *start, void *end, uint32_t region);

#endif
