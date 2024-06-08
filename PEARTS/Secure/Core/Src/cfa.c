// //#include<cfa_engine.h>
// #include<secure_nsc.h>
// #include<string.h>
// //#include<tim.h>
// #include"stm32l552xx.h"
// //#include "blake2s.h"
// //#include"usart.h"

// #define REPET_APPLICATION 1 // 100 for tests

// //#define ENERGY_EXPERIMENT 1

// CFA_ENGINE_REPORT CFreport;
// CFA_ENGINE_CONFIG CFconfig;
// CFA_PERFORMANCE_REPORT CFperformance;

// funcptr_NS  ns_interrupt_handler;

// extern void *pAttestationFunctionCallback;
// extern void *pInterruptHandlerCallback;

// uint32_t calculate_report_size(){
// 	return (CFreport.log_counter*4 + BLAKE2B_OUTBYTES + 2);
// }

// /* @ Add description
// * @
// */
// CFA_StatusTypeDef cfae_register_application_memory_range(uint32_t memory_init, uint32_t memory_end){
// 	if  (memory_end <= memory_init)  // ADD MORE RESTRICTIONS
// 		return ERROR_REGISTERING_APP_MEM;

// 	CFconfig.app_memory_range[0] = memory_init;
// 	CFconfig.app_memory_range[1] = memory_end;
// 	return SUCCESS;
// }

// /* @ Add description
// * @
// */
// CFA_StatusTypeDef cfae_initialize_cf_engine(){
// 	// Test if it is already initialized
// 	if (INITIALIZATION_STATUS) return SUCCESS;

// 	// ADD RESTRICTIONS
// 	// Set the address range  of the application // TEMP
// 	cfae_register_application_memory_range(0x080401f8,0x08040c1c); // TEMP

// 	// Initialize application pointer as NULL -> Must be registered by the ns world
// 	CFconfig.application = NULL;

// 	// Set initialized flag
// 	CLEAR_ATTESTATION_STATUS();
// 	SET_INITIALIZATION_STATUS();
// 	return SUCCESS;
// }

// CFA_StatusTypeDef cfae_check_application_bounds(){
// 	if ((uint32_t)CFconfig.application < (uint32_t)CFconfig.app_memory_range[1] &&
// 		(uint32_t)CFconfig.application >= (uint32_t)CFconfig.app_memory_range[0])
// 		return SUCCESS;
// 	return ERROR_APPLICATION_OUT_OF_B;
// }

// /* @ Add description
// * @
// */
// CFA_StatusTypeDef cfae_register_application(){
// 	// Check if app function was registered
// 	if (pAttestationFunctionCallback != NULL){ // ADD MORE RESTRICTIONS
// 		// register the ns application in the attestation structure
// 		CFconfig.application = (funcptr_NS)  pAttestationFunctionCallback;
// 		return SUCCESS;
// 	}
// 	return ERROR_REGISTERING_APP;
// }

// /* @ Add description
// * @
// */
// CFA_StatusTypeDef cfae_add_new_log_entry(uint32_t value){

// 	// Check if LAC is true
// 	if (!LAC) return ERROR_LAC_LOCKED;

// 	// Check if the CFlog is full
// 	if (CFreport.log_counter >= MAX_CF_LOG_SIZE)
// 		return ERROR_LOG_FULL;

// 	// Loop optimization
// 	if (CFconfig.last_log_entry == value){
// 		CFconfig.last_log_entry_counter ++;
// 		return SUCCESS;
// 	}

// 	// End of loop optimization
// 	if (CFconfig.last_log_entry_counter > 0){
// 		CFreport.CFLog[CFreport.log_counter] = CFconfig.last_log_entry_counter ;
// 		CFreport.log_counter ++;
// 		CFconfig.last_log_entry_counter = 0;
// 		return SUCCESS;
// 	}

// 	// Simply add the new entry if not in loop optimization
// 	CFreport.CFLog[CFreport.log_counter] = value;
// 	CFreport.log_counter ++;
// 	CFconfig.last_log_entry  = value;
// 	return SUCCESS;
// }

// void cfae_clean_attestation_structure(){
// 	CFconfig.last_log_entry_counter = 0;
// 	CFconfig.last_log_entry = -1;
// 	memset((void*)&CFreport,0,sizeof(CFA_ENGINE_REPORT));
// }

// /* @ Add description
// * @
// */
// /////////////////////////////////////////////////////////////////////

// CFA_StatusTypeDef cfae_run_attestatation(){
// 	uint64_t time_hp;
// 	uint64_t time_lp;
// 	uint32_t time_core;

// 	if(!INITIALIZATION_STATUS)
// 		return ERROR_ATT_STRUCK_NOT_INIT;

// 	// Check if attestation is already active
// 	if(ATTESTATION_STATUS)
// 		return ERROR_ATT_ALREADY_ACTIVE;
// 	SET_ATTESTATION_STATUS();

// 	// check if the application to be attested is registered
// 	if(CFconfig.application == NULL)
// 		return ERROR_NO_REGISTERED_APP;

// 	CFA_StatusTypeDef cfas;

// 	// Check if application pointer in inside the memory bound
// //	if ((cfas = cfae_check_application_bounds()))
// //		return cfas;

// #if defined ACTIVATE_MEMHASH_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
// 	TIM_reset_clock_precision();
// 	time_hp = (uint64_t) TIM_measure_clock_precise();
// #endif

// 	// Generate the binary hash of app before the execution
// 	if ((cfas = cfae_update_memhash()))
// 		return cfas;

// #if defined ACTIVATE_MEMHASH_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
// 	CFperformance.hash_memory_time = (uint64_t) TIM_measure_clock_precise() - time_hp;
// #endif

// 	// Update challange
// 	if ((cfas = cfae_update_challange()))
// 		return cfas;

// 	// Clean cfreport
// 	cfae_clean_attestation_structure();

// #if defined ACTIVATE_ENERGY_MEASUREMENT_TRIGGER && defined ACTIVATE_PERFORMANCE_EVAL
// #if ACTIVATE_ENERGY_MEASUREMENT_TRIGGER == 1
// 	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
// 	delay(100)
// #endif
// #endif


// #if defined ACTIVATE_APLICATION_EVAL && defined ACTIVATE_PERFORMANCE_EVAL
// #if ACTIVATE_PERFORMANCE_EVAL==1
// 	TIM_reset_clock_precision();
// 	time_hp = (uint64_t) TIM_measure_clock_precise();
// #endif
// #endif

// 	//Enable interrupts
// 	__enable_irq();

// 	// Call ns application "REPET_APPLICATION" times
// 	for(int k=1;k<REPET_APPLICATION;k++)
// 		CFconfig.application();

// 	//Disable interrupts
// 	__disable_irq();

// #if defined ACTIVATE_ENERGY_MEASUREMENT_RS && defined ACTIVATE_PERFORMANCE_EVAL
// #if ACTIVATE_ENERGY_MEASUREMENT_RS == 1
// 	NVIC_SystemReset();
// #endif
// #endif

// #if defined ACTIVATE_APLICATION_EVAL && defined ACTIVATE_PERFORMANCE_EVAL
// #if ACTIVATE_PERFORMANCE_EVAL==1
// 	CFperformance.application_time = (uint64_t) TIM_measure_clock_precise() - time_hp;
// #endif
// #endif

// #if defined ACTIVATE_SIG_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
// 	TIM_reset_clock_precision();
// 	time_hp = (uint64_t) TIM_measure_clock_precise();
// #endif

// 	// Sign the final report
// 	if ((cfas = cfae_sign_report()))
// 		return cfas;

// #if defined ACTIVATE_MEMHASH_MEASUREMENT && defined ACTIVATE_PERFORMANCE_EVAL
// 	CFperformance.signature_time = (uint64_t) TIM_measure_clock_precise() - time_hp;
// #endif

// 	// Send final report to vrf
// 	if ((cfas = cfae_send_report()))
// 		return cfas;

// 	// Set Attestation as inactive
// 	CLEAR_ATTESTATION_STATUS();

// 	return SUCCESS;
// }

// /* @ Add description
// * @
// */
// CFA_StatusTypeDef cfae_initialize_attestation(){

// 	CFA_StatusTypeDef cfas;

// 	// Wait for attestation request
// 	if (( cfas = cfae_wait_for_attestation_request())){
// 		return cfas;
// 	}

// 	//Disable interrupts
// 	__disable_irq();

// 	// run attestation
// 	cfas = cfae_run_attestatation();

// #ifdef ACTIVATE_PERFORMANCE_EVAL
// 	// Send Performance Report
// 	if ((cfas = cfae_send_performance_evaluation()))
// 		return cfas;
// #endif

// 	//Enable interrupts
// 	__enable_irq();

// #if defined ACTIVATE_ENERGY_MEASUREMENT_RS && defined ACTIVATE_PERFORMANCE_EVAL
// #if ACTIVATE_ENERGY_MEASUREMENT_RS == 2
// 	NVIC_SystemReset();
// #endif
// #endif

// 	return SUCCESS;
// }

#include <cfa.h>
#include <string.h>
#include <hash.h>
#include <general_configs.h>
#include <messages.h>
/* Externs */
extern HASH_HandleTypeDef hashHandler;

// Global variables
CFA_ENGINE_CONFIG       CFconfig;
CFA_ENGINE_REPORT       CFreport;
CFA_PERFORMANCE_REPORT  CFperformance;
CFA_APPLICATION_LIST	CFapplication_list;

/* Log management functions */
void cfa_append_32bits(uint32_t val);
void cfa_append_1bit_1(void);
void cfa_append_1bit_0(void);
void cfa_append_16bits(uint16_t val);

/* Structs general management functions */
void cfa_clear_report(void);
void cfa_clear_performance_report(void);
void cfa_clear_config(void);

/* Update Attestation Configs*/
void cfa_create_new_key(void);
void cfa_set_compression_rate(uint32_t cr);

/* Hash Related Functions */
void cfa_init_hash_handler(HASH_HandleTypeDef * hashHandler,uint8_t * key, uint32_t key_size, uint32_t data_type);
CFA_StatusTypeDef cfa_hash_report();
CFA_StatusTypeDef cfa_mac_report();
CFA_StatusTypeDef cfa_hash_buffer(); // Compression related
HASH_HandleTypeDef* cfa_get_hash_handler(void); 


/* Register functions */
CFA_StatusTypeDef cfa_register_application(funcptr_NS application, char* appname, uint32_t app_init_addr, uint32_t app_end_addr);
CFA_StatusTypeDef cfa_register_interrupt_handler(funcptr_NS interrupt_handler);

/* Initialization Functions */
CFA_StatusTypeDef cfa_initialize_cf_engine(void);
CFA_StatusTypeDef cfa_run_application(char * appname);

/* ****************************************************************************** */
/* **********************  LOG MANAGEMENT FUNCTIONS ************************** */
/* ****************************************************************************** */

/**
 * @brief  Append 32 bit to the report
 * @retval None
 */
void cfa_append_32bits(uint32_t val){
    // dont consider offset for now
    if(CFreport.log_counter >= MAX_CF_LOG_SIZE) return;
    CFreport.CFLog[CFreport.log_counter] = val;
    CFreport.log_counter ++;
    return;
}

void cfa_append_16bits(uint16_t val){
    //todo
}

void cfa_append_1bit_1(void){
    //todo
}

void cfa_append_1bit_0(void){
    //todo
}

/* ****************************************************************************** */
/* **********************  STRUCTS GENERAL MANAGEMENT ************************** */
/* ****************************************************************************** */

/**
 * @brief  Clear all the report struct
 * @retval None
 */
void cfa_clear_report(void){
    memset((void*)&CFreport,0,sizeof(CFA_ENGINE_REPORT));
}

/**
 * @brief  Clear all the performance report struct
 * @retval None
 */
void cfa_clear_performance_report(void){
    memset((void*)&CFperformance,0,sizeof(CFA_PERFORMANCE_REPORT));
}

/**
 * @brief  Clear all the config struct
 * @retval None
 */
void cfa_clear_config(void){
    memset((void*)&CFconfig,0,sizeof(CFA_ENGINE_CONFIG));
}


/**
 * @brief  Clear all the config struct
 * @retval None
 */
void cfa_load_cfa_key(void){
    // generate 28bytes key
    
    char key[29] = "12345678901234567890123ASDA4";
    for(int i=0;i<28;i++){
        CFconfig.key[i] = key[i];
    }
}

/* ****************************************************************************** */
/* **********************  UPDATE ATTESTATION CONFIG ************************** */
/* ****************************************************************************** */

/**
 * @brief  Creates a new key for the attestation using RNG
 * @retval None
 */
void cfa_create_new_key(void){
    if ( hash_genKey(CFconfig.key) != HASH_OK)
        Error_Handler();
    return;
}

/**
 * @brief  Set Engine Config : compression rate
 * @retval None
 */
void cfa_set_compression_rate(uint32_t cr){
    if (cr > COMPRESSION_RATE_MAX) 
        cr = COMPRESSION_RATE_MAX;
    CFconfig.compression_rate = cr;
    return;
}

/* ****************************************************************************** */
/* **********************  HASH RELATED FUNCTIONS ************************** */
/* ****************************************************************************** */

///* TEMP test func */
// byte_to_hex(uint8_t byte, uint8_t* hex) {
//    hex[0] = (byte >> 4) & 0xF;
//    hex[1] = byte & 0xF;
//    for (int i = 0; i < 2; i++) {
//        if (hex[i] < 10) {
//            hex[i] += '0';
//        } else {
//            hex[i] += 'A' - 10;
//        }
//    }
//    return hex;
//}
//

CFA_StatusTypeDef cfa_hash_memory_range(HASH_HandleTypeDef *hashHandler,uint8_t * output, uint32_t memory_init, uint32_t memory_end)
{
	// correct memory_end_offset
	memory_end += 2;

	// memory_end ;
    uint32_t memory_range = memory_end - memory_init;

    __disable_irq();
#ifdef HASH_ENGINE_AVAILABLE
    hash_SHA224Diggest(hashHandler,(uint8_t *) memory_init, memory_range, output);

#else
    //todo
#endif
    __enable_irq();

    
    return SUCCESS;
}

/**
 * @brief  Return handler to the hash engine HASH_HandleTypeDef
 * @retval hashHandler
 */
HASH_HandleTypeDef *cfa_get_hash_handler(void){
    return &hashHandler;
}

/**
 * @brief  Set Engine Config : compression rate
 * @retval None
 */
void cfa_init_hash_handler(HASH_HandleTypeDef * hashHandler,uint8_t * key, uint32_t key_size, uint32_t data_type){
	// cfa_init_hash_handler(hashHandler,key,KEYSIZE,HASH_DATATYPE_8B);
	hash_init_hash_handler(hashHandler,key,key_size,data_type);
}

/**
 * @brief  Hash the full report 
 * @retval None
 */
CFA_StatusTypeDef cfa_hash_report(){
    //todo
    return SUCCESS;
}

/**
 * @brief  Hash the buffer according to the compression rate
 * @retval None
 */
CFA_StatusTypeDef cfa_hash_buffer(){
    //todo
    return SUCCESS;
}


/* ****************************************************************************** */
/* **********************  VALIDATION CHECKS   ************************** */
/* ****************************************************************************** */

/**
 * @brief  Check if the application pointer is inside the memory range and if the application registration is valid
 * @retval None
 */

uint8_t _check_memory_range(funcptr_NS application, char* appname, uint32_t app_init_addr, uint32_t app_end_addr){
    if (application == NULL) return ERROR;
    if (CFapplication_list.app_counter >= MAX_APP_LIST_SIZE) return ERROR;
    if ((uint32_t)application >= (uint32_t)app_end_addr || (uint32_t)CFconfig.application < (uint32_t)app_init_addr)
        return ERROR;
    return SUCCESS;
}

/* ****************************************************************************** */
/* **********************  REGISTER FUNCTIONS   ************************** */
/* ****************************************************************************** */

/**
 * @brief  Register new application
 * @retval None
 */
CFA_StatusTypeDef cfa_register_application(funcptr_NS application, char* appname, uint32_t app_init_addr, uint32_t app_end_addr){

    // check for error
    if (_check_memory_range(application,appname, app_init_addr,app_end_addr)) Error_Handler();

    // register application
    CFapplication_list.application[CFapplication_list.app_counter] = application;
    strcpy((char*)CFapplication_list.app_name[CFapplication_list.app_counter],(char*)appname);
    CFapplication_list.app_counter ++;

    return SUCCESS;
}

/**
 * @brief  Register new interrupt handler
 * @retval None
 */
CFA_StatusTypeDef cfa_register_interrupt_handler(funcptr_NS interrupt_handler){
    //todo
    return SUCCESS;
}


/* ****************************************************************************** */
/* **********************  MEMORY / INTERRUPT CONTROL FUNCTIONS   *************** */
/* ****************************************************************************** */



/* ****************************************************************************** */
/* **********************  REPORT RELATED FUNCTIONS   ************************** */
/* ****************************************************************************** */



CFA_StatusTypeDef cfa_send_report_serial(){

	uint32_t report_size = (sizeof(CFreport) - (4*MAX_CF_LOG_SIZE)) + CFreport.log_counter * 4;
    char mess[10];

	send_MESSAGE("\nCFA Log Size:\n",COLOR_CYAN);
	send_MESSAGE_W((char*)itoa(CFreport.log_counter+1,mess,10), strlen(mess));
	send_MESSAGE("\nCFA Log Data:\n",COLOR_CYAN);
	send_MESSAGE_W((char*)&CFreport, report_size);
	send_MESSAGE("\nEND OF LOG:\n",COLOR_CYAN);
	CFreport.log_counter = 0;

#if PERFORMANCE_EXPERIMENT==ACTIVATED
	//todo
	// send performance
#endif

	return SUCCESS;
}


CFA_StatusTypeDef cfa_send_final_report(){
    if (CFreport.log_counter == 0) return SUCCESS;
    cfa_send_report_serial();
    //todo
    return SUCCESS;
}

/* ****************************************************************************** */
/* **********************  INITIALIZATION FUNCTIONS   ************************** */
/* ****************************************************************************** */

CFA_StatusTypeDef cfa_initialize_structs(){
	cfa_clear_report();
	cfa_clear_performance_report();
	cfa_clear_config();

	cfa_load_cfa_key();
    return SUCCESS;
}


CFA_StatusTypeDef cfa_initialize_cf_engine(){
    //todo
    return SUCCESS;
}

CFA_StatusTypeDef cfa_setup_attestation_beg(){
    __disable_irq();

    // hash non-secure memory
    HASH_HandleTypeDef *hashHandler;
    hashHandler = cfa_get_hash_handler();
    cfa_init_hash_handler(hashHandler,CFconfig.key,KEYSIZE,HASH_DATATYPE_8B);
    cfa_hash_memory_range(hashHandler,CFreport.memhash,NONSECURE_FLASH_START,NONSECURE_FLASH_END);

    // set attestation status to active
    SET_ATTESTATION_STATUS();

    // configure memory security features
    //todo

    //config interrupts
    //todo

    //config IO
    //todo

    __enable_irq();
    return SUCCESS;
}

CFA_StatusTypeDef cfa_setup_attestation_end(){
    __disable_irq();

    // set attestation status to deactive
    CLEAR_ATTESTATION_STATUS();

    __enable_irq();
    return SUCCESS;
}

CFA_StatusTypeDef cfa_run_application(char* appname){
    
    int application_id = -1;
    
    // find application
    for (int i=0;i<CFapplication_list.app_counter;i++){

        if (strcmp((char*)CFapplication_list.app_name[i],(char*)appname) == 0){
            application_id = i;
            break;
        }
    }
    if (application_id == -1) return ERROR_APP_NOT_FOUND;
    funcptr_NS nsApp = CFapplication_list.application[application_id];
    //todo
    
    #ifdef ACTIVATE_PERFORMANCE_EVAL
        //todo
    #endif

    send_LOG("Starting the execution of the application", NULL); // PROBABLY HAS A BUFFER OVERFLOW

    // setup attestation startup
    if (cfa_setup_attestation_beg() != SUCCESS) Error_Handler();
    // run application
    nsApp();
    // setup attestation end
    if (cfa_setup_attestation_end() != SUCCESS) Error_Handler();

    #ifdef ACTIVATE_PERFORMANCE_EVAL
        //todo
    #endif

    //todo
    
//    if (cfa_mac_report() != SUCCESS) Error_Handler();

    //todo

    if (cfa_send_final_report() != SUCCESS) Error_Handler();

    return SUCCESS;
}

  






