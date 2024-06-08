#include <secure_terminal.h>
#include <string.h>
#include <messages.h>

Messages M1;

/*   **************** MESSAGE 1 : MAIN TERMINAL MESSAGE ***************************  */
#define NUM_COMMANDS1 5
#define NUM_MESSAGES1 5

// define the messages that the terminal transmit
const char* MESSAGES1[] = {"\trun_attestation","\tlist_apps","\thelp","\texit", "\trun_rtpox"
};

// define the commands that the terminal can receive as a string
const char* COMMAND_LIST1[] = {"run_attestation","list_apps","help","exit","run_rtpox"
};


void (*COMMAND_FUNCTIONS1[NUM_COMMANDS1])() = {run_attestation,list_apps,help,eexit,run_rtpox
};

/*   ********************************* END MESSAGE 1 *****************************************  */

DebugStatus debugStatusHandler;

void st_initTerminalDebugStatus(void){
    // initialize debug status
    debugStatusHandler.NonSecureWorldInitialized = 0;
}

void setMessages(Messages * M, const char* message[], const char* commandList[], void(** commandFunctions)(char**,int), uint8_t nMessages, uint8_t nFunctions){
    M->message = message;
    M->commandList = commandList;
    M->commandFunctions = commandFunctions;
    M->nMessages = nMessages;
    M->nFunctions = nFunctions;
}

void createMessages(){
    // while(1){}
    setMessages(&M1,MESSAGES1,COMMAND_LIST1,COMMAND_FUNCTIONS1,NUM_MESSAGES1,NUM_COMMANDS1);
}

void printMenu(Messages * M) {
    // print the message to the serial "Avaliable commands:"
    HAL_UART_Transmit(&hlpuart1, (uint8_t*)"\nAvailable commands:\r\n", 22, HAL_MAX_DELAY);

    // print the list of available commands to the serial
    for (int i = 0; i < M->nMessages; i++) {
        HAL_UART_Transmit(&hlpuart1, (uint8_t*)M->message[i], strlen(M->message[i]), HAL_MAX_DELAY);
        HAL_UART_Transmit(&hlpuart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
    }
}

#define BUFFERMAX 50

void parse_command(char buffer[50], int* num_args,char args[3][20]) {
    char* token = strtok(buffer, " \n");
    int i = 0;
    while (token != NULL && i < 3) {
        strncpy(args[i], token, 20);
        args[i][19] = '\0';
        token = strtok(NULL, " \n\0");
        i++;
    }
    *num_args = i;
}

void readMenuAndExecuteCommand(Messages * M) {
    
    char args[3][20];
    char * cmd = args[0];
    int num_args;
    printMenu(M);

    // define a buffer to store the received command
    char buffer[BUFFERMAX];
    memset(buffer, 0, sizeof(buffer)); // clear the buffer

    // read from serial until a newline character is received
    int i = 0;
    char c;

    // read systick
    uint32_t tickstart = HAL_GetTick();
//
//
//    __disable_irq();
    
    while (1) {

        if(HAL_GetTick()-tickstart > 0xFFFF){
            __enable_irq();
            return;
        }

        if(HAL_UART_Receive(&hlpuart1, (uint8_t*)&c, 1, 0xFFF)==HAL_TIMEOUT){
            continue;
        }

        if (c == '\n') {
            break;
        }
        buffer[i++] = c;
        if (i == BUFFERMAX) break;
    }
    
//    __enable_irq();

    if (strlen(buffer) > 1)
        send_LOG(buffer, COLOR_YELLOW);

    // parse the received command
    parse_command(buffer, &num_args, args); 

    if (num_args == 0) return;

        // print all args
    for(int i=0;i<num_args;i++){
        send_LOG(args[i], COLOR_MAGENTA);
    }


    // compare the received command with the list of commands
    for (int i = 0; i < M->nMessages; i++) {
        if (strcmp(cmd, M->commandList[i]) == 0) {
            // execute the corresponding function
        	( M->commandFunctions[i])(args,(int)num_args);
            return;
        }
    }
    
    // print command not found to the serial
    if ( strcmp(buffer,"\n") != 0){
        addColor("Command not found\r\n", buffer, sizeof(buffer), COLOR_RED);
    }

	HAL_UART_Transmit(&hlpuart1,(uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

}


// remove
#include<rtpox_its.h>


void st_open_terminal(void){
    // initialize the terminal
    rtpox_its_setAllInterruptsAsDumb();

	__asm__(" .align 4");
     // Initialize menu information
     createMessages();
    
     // read and execute commands until the eexit command is received
     while (1) {
         readMenuAndExecuteCommand(&M1);
         // sleep(1);
         HAL_Delay(100);
     }
}

