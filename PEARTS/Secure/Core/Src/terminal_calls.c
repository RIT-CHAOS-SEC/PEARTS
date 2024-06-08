// #include "terminal_calls.h"
// #include "secure_terminal.h"
#include "main.h"
#include <string.h>
#include <messages.h>
#include <terminal_calls.h>
#include <cfa.h>
#include <general_configs.h>

extern CFA_APPLICATION_LIST CFapplication_list;

char help_message[] =
"\n"
"######################################\n"
"################ HELP ################\n"
"######################################\n\n"
"\tlogin: login to the system\n\n"
"\tlogout: logout from the system\n\n"
"\tview: view the current user\n\n"
"\tedit: edit the current user\n\n"
"\tdelete: delete the current user\n\n"
"\tcreate: create a new user\n\n"
"\thelp: display this message\n\n"
"\texit: exit the terminal\n\n"
"\trestart: restart the device\n\n"
"\trun_attestation: run the attestation process\n"
"\t\tUsage: run_attestation <name_of_the_app>\n\n"
"\t\tExtra Info: To know list available apps, run \"list apps\">\n\n";

extern UART_HandleTypeDef hlpuart1;

void donothing(){
	__asm("\tnop\n");
	return;
}

void login(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    // implementation of login command
    #if DEBUG==ACTIVATED
    send_LOG("Entered login function\n",NULL);
    #endif
}

void logout(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    #if DEBUG==ACTIVATED
    send_LOG("Entered logout function\n",NULL);
    #endif
    // implementation of logout command
}

void view(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    #if DEBUG==ACTIVATED
    send_LOG("Entered view function\n",NULL);
    #endif
    // implementation of view command
}

void edit(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    #if DEBUG==ACTIVATED
    send_LOG("Entered edit function\n",NULL);
    #endif
    // implementation of edit command
}

void del(char  args[][MAX_COMMAND_LENGTH], int num_args){
    #if DEBUG==ACTIVATED
    send_LOG("Entered del function\n",NULL);
    #endif
    // implementation of delete command
}

void create(char  args[][MAX_COMMAND_LENGTH], int num_args){
    #if DEBUG==ACTIVATED
    send_LOG("Entered create function\n",NULL);
    #endif
    // implementation of create command
}

void help(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    #if DEBUG==ACTIVATED
    send_LOG("Entered help function\n",NULL);
    #endif
    // send   help_message  to serial
    HAL_UART_Transmit(&hlpuart1,(uint8_t*)help_message, strlen(help_message), HAL_MAX_DELAY);

}

void restart(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    #if DEBUG==ACTIVATED
    send_LOG("Entered restart function\n",NULL);
    #endif
    // restart the device
    HAL_NVIC_SystemReset();
}

/***************************** BEING USED *****************************/

void eexit(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    #if DEBUG==ACTIVATED
    send_LOG("Entered exit function\n",NULL);
    #endif
    return;
}

void run_attestation(char  args[][MAX_COMMAND_LENGTH], int num_args) {
//    #if DEBUG==ACTIVATED
    send_LOG("Entered run_attestation function\n",NULL);
//    #endif
    // implementation of run_attestation command

    char message[50];

    // print all args
    for(int i=0;i<num_args;i++){
        send_LOG(args[i], COLOR_RED);
    }

    if (num_args < 2) {
        // print invalid command in red
        addColor("Invalid command!! Missing arg \n",message,50, COLOR_RED);
        HAL_UART_Transmit(&hlpuart1,(uint8_t*)message, strlen(message), HAL_MAX_DELAY);
        return;
    }
    else {
        // use the first token as the first parameter
        addColor(args[1],message, sizeof(message), COLOR_RED);
        HAL_UART_Transmit(&hlpuart1,(uint8_t*)strcat("[LOG]: ", message), strlen(message) + strlen("[LOG]: "), HAL_MAX_DELAY);
    }
    cfa_run_application(args[1]);
}

void run_rtpox(char  args[][MAX_COMMAND_LENGTH], int num_args) {
    send_LOG("Entered run_attestation function\n",NULL);
    char message[50];
}


void list_apps(char  args[][MAX_COMMAND_LENGTH], int num_args){
    #if DEBUG == ACTIVATED
    send_LOG("Entered run_attestation function\n",NULL);
    #endif

    if(CFapplication_list.app_counter == 0){
        send_MESSAGE("No Registered Applications", COLOR_RED);
        return;
    }

    // list the available apps in the CFA_APPLICATION_LIST
    send_MESSAGE("List of Registered Applications : \n", NULL);
    for(int i = 0; i < CFapplication_list.app_counter; i++){
        send_MESSAGE(strcat((char*)CFapplication_list.app_name[i], "\n"), COLOR_BLUE);
    }
    send_MESSAGE("\n", NULL);
    return;
}
