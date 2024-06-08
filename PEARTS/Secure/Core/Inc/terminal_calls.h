#ifndef __TERMINAL_CALLS__H__
#define __TERMINAL_CALLS__H__

#include "stm32l5xx_hal.h"

#define MAX_COMMAND_LENGTH 20

void login(char  args[][MAX_COMMAND_LENGTH], int num_args);
void logout(char  args[][MAX_COMMAND_LENGTH], int num_args);
void view(char  args[][MAX_COMMAND_LENGTH], int num_args);
void edit(char  args[][MAX_COMMAND_LENGTH], int num_args);
void del(char  args[][MAX_COMMAND_LENGTH], int num_args);
void create(char  args[][MAX_COMMAND_LENGTH], int num_args);
void help(char  args[][MAX_COMMAND_LENGTH], int num_args);
void restart(char  args[][MAX_COMMAND_LENGTH], int num_args);
void eexit(char  args[][MAX_COMMAND_LENGTH], int num_args);
void run_attestation(char  args[][MAX_COMMAND_LENGTH], int num_args);
void list_apps(char  args[][MAX_COMMAND_LENGTH], int num_args);
void run_rtpox(char  args[][MAX_COMMAND_LENGTH], int num_args);

void donothing();

#endif  //!__TERMINAL_CALLS__H__
