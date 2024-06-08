#include<main.h>
#include <rtpox.h>
#include <string.h>
#include "rtpox_types.h"

extern rtpox_config_t  rtpox_config;

// FAULT POINTS
uint32_t secure_point = 0;
uint32_t SECURE_ENTRY_Systick = 0;
int32_t SECURE_ENTRY_PendSV = 0;
int32_t SECURE_ENTRY_Resume = 0;


uint32_t rtpox_it_getResume(){ return (uint32_t)&SECURE_ENTRY_Resume;}

// #define RTPOX_ELASTIC  void __attribute__((section(".rtpox_elastic")))

int a = 0;
void rtpox_its_dummy(){
    a++;
};

void setNSVTOR();
void rtpox_mapNewVTOR();
void rtpox_mapNewNSVTOR();
void rtpox_NS_systickInterrupt (void);

// Read the ICSR register to find the active interrupt
#define rtpox_its_findInteruptSource() (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

/* ############################## Interrupt Configs ############################################# 
  _____       _                             _           _____             __ _       
 |_   _|     | |                           | |         / ____|           / _(_)      
   | |  _ __ | |_ ___ _ __ _ __ _   _ _ __ | |_       | |     ___  _ __ | |_ _  __ _ 
   | | | '_ \| __/ _ \ '__| '__| | | | '_ \| __|      | |    / _ \| '_ \|  _| |/ _` |
  _| |_| | | | ||  __/ |  | |  | |_| | |_) | |_       | |___| (_) | | | | | | | (_| |
 |_____|_| |_|\__\___|_|  |_|   \__,_| .__/ \__|       \_____\___/|_| |_|_| |_|\__, |
                                     | |                                        __/ |
                                     |_|                                       |___/ 
################################################################################################# */ 

#define ST32L5_MAX_INTERRUPTS 107+16
#define ST32L5_MAX_INTERRUPTS_blocks ((ST32L5_MAX_INTERRUPTS)/32) + 1

extern uint32_t _rtpox_isr_start;
extern uint32_t _rtpox_isrns_start;

// uint32_t * _rtpox_isr_start[ST32L5_MAX_INTERRUPTS];

typedef struct{
	uint32_t * new_SVTOR;
    uint32_t * old_SVTOR;
	uint32_t * NSVTOR;
	uint32_t * new_NSVTOR;
	uint32_t rtpox_interrupts[ST32L5_MAX_INTERRUPTS_blocks];
	struct{
		uint32_t ITNS[ST32L5_MAX_INTERRUPTS_blocks];
	} active;
	struct{
		uint32_t ITNS[ST32L5_MAX_INTERRUPTS_blocks];
	} deactive;
    uint32_t flags;
} rtpox_interrupt_config_t;

rtpox_interrupt_config_t interrupt_config;

void rtpox_its_setAllInterruptsSecure(){
    memset(NVIC->ITNS,0,8*sizeof(uint32_t));
}

#define SAU_NONSECURE_REGION 	0x10U
#define SAU_SECURE_REGION 		0x00U
#define SAU_ACTIVATE 	 		0x01U
#define SAU_NONSECURE_CALLABLE  0x10U
#define SAU_REGION_ENABLED		0x01U

void rtpox_initITS(){

    // save current NSVTOR address
    interrupt_config.NSVTOR = (uint32_t *)SCB_NS->VTOR;

    // change VTOR address to reserved _rtpox_isr_start
    interrupt_config.new_SVTOR = (uint32_t*) &_rtpox_isr_start;
    interrupt_config.new_NSVTOR = (uint32_t*) &_rtpox_isrns_start;

    //todo:remove this lne
    // interrupt_config.new_NSVTOR = interrupt_config.NSVTOR;

    // offset the address until divisibe by 128
    while((uint32_t) interrupt_config.new_SVTOR % 128 != 0) interrupt_config.new_SVTOR++;

    // fill the new VTOR with rtpox functions
    rtpox_mapNewVTOR();
    rtpox_mapNewNSVTOR();

    // save current VTOR address
    interrupt_config.old_SVTOR = (uint32_t *)SCB->VTOR;

    // initialization status to 1
    interrupt_config.flags |= 0x1U;

    // copy current ITNS
    for (int i = 0; i < ST32L5_MAX_INTERRUPTS_blocks; i++){
        interrupt_config.deactive.ITNS[i] = NVIC->ITNS[i];
    }

    // set all the exceptions as secure interrupts when active
    interrupt_config.active.ITNS[0] &= ~(0xFFFFU);
    // set all the exceptions as secure interrupts when deactive, except the systick, syscall and pendSV 
    interrupt_config.deactive.ITNS[0] &= ~(0xFFFFU);
    interrupt_config.deactive.ITNS[0] |= 0b1100100000000000;
}

extern uint32_t _rtpox_elastic_start,_rtpox_elastic_end;

void rtpox_its_setPoXISR(){

//	set_SAU(6,&_rtpox_elastic_start,&_rtpox_elastic_end,SAU_NONSECURE_REGION);

    SCB->VTOR = (uint32_t)interrupt_config.new_SVTOR;
    SCB_NS->VTOR = (uint32_t)interrupt_config.new_NSVTOR;
}

void rtpox_its_unsetPoXISR(){
    SCB->VTOR = (uint32_t)interrupt_config.old_SVTOR;
    SCB_NS->VTOR = (uint32_t)interrupt_config.NSVTOR;
}

volatile void  rtpox_its_setAllInterruptsAsDumb(){
    if (interrupt_config.flags == 0x0U){
        rtpox_initITS();
    }
    __disable_irq();
    rtpox_its_setAllInterruptsSecure();
    rtpox_its_setPoXISR();
    __enable_irq();
}

void rtpox_interruptDispatcher_HardFault(){
	__disable_irq();
	// Dispatcher for the faults
	while(1){
	}
	__enable_irq();
}

void rtpox_interruptDispatcher_Reset(){
	__disable_irq();
    // Reset device
    NVIC_SystemReset();
	while(1){/*not supposed to reach here*/}
}


#define SFAR_INVEP 	0U // INvalid entry point
#define HFSR_FORCED 30
#define PENDSVSET	28
#define DFSR_HALTED 0

#define FALSE 	0
#define TRUE	1


// Non secure trampoline to return from exception
void __attribute__((section(".rtpox_elastic_ns_flash"),naked ))  __return_NS(){
	__ASM volatile("bx lr\n\t");
}

// Secure trampoline to return from exception
void __attribute__((naked)) __return_S(){
    __ASM volatile("bx lr\n\t");
}

uint32_t rtpox_active = FALSE;

void rtpox_its_endContext(){
	rtpox_its_unsetPoXISR();

}

void rtpox_interruptDispatcher_External(){
	uint32_t interrupt_id = rtpox_its_findInteruptSource();
	// Dispatcher for the faults
	while(1){
	}
}

void rtpox_mapNewNSVTOR(){
    // Map the new NSVTOR with the rtpox functions
    for (int i = 0; i < 16; i++)
        interrupt_config.new_NSVTOR[i] = (uint32_t)interrupt_config.NSVTOR[i];
//    interrupt_config.new_NSVTOR[15] = (uint32_t)rtpox_interruptDispatcher_SystickNS;
    interrupt_config.new_NSVTOR[15] = ((uint32_t)&SECURE_ENTRY_Systick|01U);//((uint32_t) (rtpox_NS_systickInterrupt));
//    interrupt_config.new_NSVTOR[14] = ((uint32_t)&SECURE_ENTRY_PendSV|01U);
}

void rtpox_mapNewVTOR(){
    for (int i = 16; i < ST32L5_MAX_INTERRUPTS; i++)
        interrupt_config.new_SVTOR[i] = (uint32_t)rtpox_interruptDispatcher_External;
}

# define SAU_ESR_RAM_REGION 		7
# define SAU_ESR_RAM_BASE_ADDRESS 	_
# define SAU_ESR_RAM_END_ADDRESS 	7

# define SAU_ESR_FLASH_REGION 		6
# define SAU_ESR_FLASH_BASE_ADDRESS 	_
# define SAU_ESR_FLASH_END_ADDRESS 	7

/* ############################## DISPATCHER FUNCTIONS ############################################# 
 _______   __       _______..______      ___   .___________.  ______  __    __   _______ .______      
|       \ |  |     /       ||   _  \    /   \  |           | /      ||  |  |  | |   ____||   _  \     
|  .--.  ||  |    |   (----`|  |_)  |  /  ^  \ `---|  |----`|  ,----'|  |__|  | |  |__   |  |_)  |    
|  |  |  ||  |     \   \    |   ___/  /  /_\  \    |  |     |  |     |   __   | |   __|  |      /     
|  '--'  ||  | .----)   |   |  |     /  _____  \   |  |     |  `----.|  |  |  | |  |____ |  |\  \----.
|_______/ |__| |_______/    | _|    /__/     \__\  |__|      \______||__|  |__| |_______|| _| `._____|
                                                                                                               
################################################################################################# */ 
// Generate big texts https://www.fancytextpro.com/BigTextGenerator/SubZero

__inline void _rtpox_deactivateSAU(uint32_t region){
    // Disable SAU
    SAU->CTRL &= ~SAU_CTRL_ENABLE_Msk;
    // Select SAU region 7
    SAU->RNR = region;
    // Disable the region
    SAU->RLAR &= ~SAU_RLAR_ENABLE_Msk;
    // Enable SAU
    SAU->CTRL |= SAU_CTRL_ENABLE_Msk;
}

__inline void _rtpox_activateSAU(uint32_t region,uint32_t base_address,uint32_t limit_address){
	// Disable SAU
	SAU->CTRL &= ~(SAU_ACTIVATE);
	// Select SAU 
	SAU->RNR  = region;
	// Set the base address
	SAU->RBAR = base_address;
	// Set the limit address
	SAU->RLAR = ((limit_address & ~(0b11111))|SAU_REGION_ENABLED|SAU_NONSECURE_REGION);
	// Enable SAU
	SAU->CTRL = SAU_NONSECURE_REGION | SAU_REGION_ENABLED;
}

__inline void _rtpox_shrinkESR(){
    _rtpox_deactivateSAU(SAU_ESR_RAM_REGION);
    _rtpox_deactivateSAU(SAU_ESR_FLASH_REGION);
}

__inline void _rtpox_expandESR(){
    _rtpox_activateSAU(SAU_ESR_RAM_REGION, SAU_ESR_RAM_BASE_ADDRESS, SAU_ESR_RAM_END_ADDRESS);
    _rtpox_activateSAU(SAU_ESR_FLASH_REGION, SAU_ESR_FLASH_BASE_ADDRESS, SAU_ESR_FLASH_END_ADDRESS);
}

#pragma GCC push_options
#pragma GCC optimize ("-O3")
void rtpox_interruptDispatcher_SecureHardFault(){
	__disable_irq();

 	rtpox_config.status.exec = RT_POX_RUNNING_APP; // todo add this to the initialization

 	// Check if the the exception was generated by invalid secure entry point
    if ((SAU->SFSR) & (1U<<SFAR_INVEP)) // todo test if a systick is pending in the NS_World
	{
        // declare returning address
    	uint32_t * return_address;

    	// get EXEC_RETURN value from LR
        uint32_t exec_return = (uint32_t)__builtin_return_address(0);

        // get return address depending if interrupt happened at handler or thread mode
        if (exec_return & (1U<<3)){
        	return_address = (uint32_t *)((uint32_t)__TZ_get_PSP_NS() + 0x18); // if no extended stacking
        }
        else{
        	return_address = (uint32_t *)((uint32_t)__TZ_get_MSP_NS() + 0x18); // if no extended stacking
        }

        // test if was triggered by systick interrupt
        if ((uint32_t)(* return_address)==(uint32_t)&SECURE_ENTRY_Systick){
        	// Expand the ESR
            _rtpox_expandESR();

        	// get current psp from Sapp
        	rtpox_config.apps_config.app[rtpox_config.apps_config.active_app].psp = __TZ_get_PSP_NS();
        	// set ns_psp to the Sapp template task one
        	__TZ_set_PSP_NS(rtpox_config.apps_config.final_ns_sp);

        	// Call the systick handler in the NS-World
			funcptr_NS systick = (funcptr_NS) interrupt_config.NSVTOR[15];
			systick();

            // Shrink the ESR
            _rtpox_shrinkESR();
        

			// check if PendSV is activate
        	if (SCB_NS->ICSR & (1U<<28)){
        		// PendSV is Activated
				// get return address from ns_psp stack
				uint32_t * return_address_psp = (uint32_t *)((uint32_t)__TZ_get_PSP_NS() + 0x18);
				// change msp return address to the trampoline
        		*return_address = ((uint32_t)__return_NS  ); // todo, change to __return_S when memory protection enable

        		// get address of xpsr
        		uint32_t * xpsr = return_address_psp + 1;
        		// reset xpsr
        		*xpsr = 0x1000000U;

        		// Test if Sapp or Sext is executing
        		if(rtpox_config.status.exec == RT_POX_RUNNING_APP){
        			// change the returning address of psp to the resume
        			*return_address_psp = ((uint32_t) rtpox_config.apps_config.app[rtpox_config.apps_config.active_app].funcs.rtpox_resume); // todo, use app_id
        			// change execution status to be Sext executing
        			rtpox_config.status.exec = RT_POX_NOT_RUNNING_APP;
        			//todo context with pendsv
        			//todo change NSVTOR to original while outside the rtpox task
                    SCB_NS->VTOR = (uint32_t)interrupt_config.NSVTOR;
                    
        		}
        		else{
        			while(1){} // trap  if RT_POX_NOT_RUNNING
        		}

			__enable_irq();
			return;
        	}
        }

        // test if was triggered by rtpox resume call
        else if((uint32_t)(* return_address)==(uint32_t)&SECURE_ENTRY_Resume){
        	// Expand the ESR
            _rtpox_shrinkESR();

        	// change status of rtpox to running
        	rtpox_config.status.exec = RT_POX_RUNNING_APP;

        	// Switch PSP
			// get current psp from Sapp template task
			rtpox_config.apps_config.final_ns_sp = __TZ_get_PSP_NS();
			// set ns_psp to the Sapp
			__TZ_set_PSP_NS(rtpox_config.apps_config.app[rtpox_config.apps_config.active_app].psp);
        	// switch NSVTOR to new NSVTOR
        	SCB_NS->VTOR = (uint32_t)interrupt_config.new_NSVTOR;

            // return from exception
            __enable_irq();
            return;
        }

        else{
            // discover which address triggered the fault
            while(1){} // trap execution, other error ...
        }

	}
    else{
    	while(1){} // trap execution, other error ...
    }
}
#pragma GCC pop_options
