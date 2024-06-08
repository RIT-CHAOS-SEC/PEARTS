/*
 * hash.c
 *
 *  Created on: Oct 24, 2023
 *      Author: nneto
 */

// #include <main.h>
#include <stm32l5xx.h>
#include "hash.h"

HashErrorStatusTypeDef hash_genKey(uint8_t * key);

hHandler hashHandler;
RNG_HandleTypeDef hrng;

void hash_init_hash_handler(HASH_HandleTypeDef * hashHandler,uint8_t * key, uint32_t key_size, uint32_t data_type){

	 hashHandler->Init.DataType = HASH_DATATYPE_32B;
	 hashHandler->Init.KeySize = KEYSIZE;
	 hashHandler->Init.pKey = (uint8_t*) key;
	 if(HAL_HASH_Init(hashHandler) != HAL_OK)
	 {
		Error_Handler();
	 }
	 return;
}

void hash_SHA224Diggest(HASH_HandleTypeDef * hhash,uint8_t * aInput, int inputSize, uint8_t * SHA224Diggest){

		if (HAL_HASHEx_SHA224_Start(hhash, aInput, inputSize, SHA224Diggest, HASHTimeout) != HAL_OK){
			Error_Handler();
		}
		while (HAL_HASH_GetState(hhash) == HAL_HASH_STATE_BUSY);

		return;
}

void hash_MACSHA224Diggest(HASH_HandleTypeDef * hhash,uint8_t * aInput, int inputSize, uint8_t * SHA224Diggest){

		if (HAL_HMACEx_SHA224_Start(hhash, aInput, inputSize, SHA224Diggest, HASHTimeout) != HAL_OK){
			Error_Handler();
		}
		while (HAL_HASH_GetState(hhash) == HAL_HASH_STATE_BUSY);

		return;
}

HashErrorStatusTypeDef hash_genKey(uint8_t * key){

	uint32_t*  walker = (uint32_t*) key;

	for (int i = 0; i < (int) KEYSIZE / 4; i++){

		if (HAL_RNG_GenerateRandomNumber(&hrng, walker ) != HAL_OK){
			return HASH_ERROR_RGN;
		}
		walker ++;
	}
	return HASH_OK;
}


void rng_init(void){

	hrng.Instance = RNG;
	hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;

	if (HAL_RNG_Init(&hrng) != HAL_OK)
	{
		Error_Handler();
	}
	return;
}


