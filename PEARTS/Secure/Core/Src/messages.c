#include "messages.h"
#include <string.h>

extern UART_HandleTypeDef hlpuart1;

void addColor(char* input, char* output, int outputSize, char * color) {
    // clear output
    memset(output, 0, outputSize);
    // add color to the input string and write to the output buffer
    strcat(output, color);
    strcat(output, input);
    strcat(output, COLOR_RESET);
}

void send_LOG(char  buffer[], char*color){
    char message[80] = "";
    int buff_size = strlen(buffer);


	if (color == NULL) color = COLOR_YELLOW;
    addColor("[LOG] : ",message,sizeof(message), COLOR_GREEN);


    HAL_UART_Transmit(&hlpuart1,(uint8_t*)message, strlen(message), HAL_MAX_DELAY);
    // send buffer to serial in green

    addColor(buffer,message,sizeof(message), color);


    HAL_UART_Transmit(&hlpuart1,(uint8_t*)message, strlen(message), HAL_MAX_DELAY);
    HAL_UART_Transmit(&hlpuart1,(uint8_t*)"\n",  1 , HAL_MAX_DELAY);

}

void send_MESSAGE(char * buffer, char*color){
	if (color == NULL) color = COLOR_YELLOW;

    char message[50];
    // send buffer to serial in green
    addColor(buffer,message,50, color);
    HAL_UART_Transmit(&hlpuart1,(uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}


void send_MESSAGE_W(char * buffer, uint32_t size){
	HAL_UART_Transmit(&hlpuart1,(uint8_t*)buffer, size, HAL_MAX_DELAY);
}
