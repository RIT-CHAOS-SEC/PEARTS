#ifndef __APP_MESSAGES__H__
#define __APP_MESSAGES__H__

#include <terminal_calls.h>

// define color codes
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN "\033[1;36m"
#define COLOR_RESET "\033[0m"



/* ******** Messages Struct ********* */
typedef struct Messages{
    const char** message;
    const char** commandList;
    void (** commandFunctions)(char[][MAX_COMMAND_LENGTH], int);
    uint8_t nMessages;
    uint8_t nFunctions;
} Messages;


/* Debug struct */

typedef struct DebugStatus{
    uint8_t NonSecureWorldInitialized;
} DebugStatus;



void send_LOG(char * buffer,  char* color);
void send_MESSAGE(char * buffer, char*color);
void send_MESSAGE_W(char * buffer, uint32_t size);

#endif
