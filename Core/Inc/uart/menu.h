#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#include <uart/command.h>

void menuInit(Command **commands, uint16_t commandsSize);
const char * menuExecuteCommand(char *commandNameWithArg);


#endif //MENU_H
