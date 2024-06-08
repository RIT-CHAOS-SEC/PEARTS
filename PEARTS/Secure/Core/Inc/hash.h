/*
 * hash.h
 *
 *  Created on: Oct 25, 2023
 *      Author: nneto
 */

#ifndef INC_HASH_H_
#define INC_HASH_H_

#define KEYSIZE ((uint32_t) 224/8)
#define HASHTimeout 0xFF
#define MODIFIERSIZE 1

typedef struct hHandler{
	HASH_HandleTypeDef hashHandler;
	uint8_t key[KEYSIZE];
	uint8_t modifier[MODIFIERSIZE];

} hHandler;

typedef uint32_t HashErrorStatusTypeDef;

enum HashStatus
{
	HASH_OK,
	HASH_ERROR,
	HASH_BUSY,
	HASH_TIMEOUT,
	HASH_ERROR_RGN
};

void hash_SHA224Diggest(HASH_HandleTypeDef * hhash,uint8_t * aInput, int inputSize, uint8_t * SHA224Diggest);

void hash_MACSHA224Diggest(HASH_HandleTypeDef * hhash,uint8_t * aInput, int inputSize, uint8_t * SHA224Diggest);

void rng_init(void);

// hHandler hash_initHashKey(void);

void hash_init_hash_handler(HASH_HandleTypeDef * hashHandler,uint8_t * key, uint32_t key_size, uint32_t data_type);

HashErrorStatusTypeDef hash_genKey(uint8_t * key);


#endif /* INC_HASH_H_ */
