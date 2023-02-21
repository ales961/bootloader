#include "menu.h"

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"

Command **menuCommands = NULL;
uint16_t menuSize;

static int32_t commandSepNameFromArg(const char *commandNameWithArg);
static void strTrim(char *str);

void menuInit(Command ** const commands, const uint16_t commandsSize) {
    if (menuCommands != NULL)
        return;
    menuCommands = (Command **) malloc(sizeof(Command *) * commandsSize);
    for (uint16_t i = 0; i < commandsSize; i++) {
        menuCommands[i] = commands[i];
    }
    menuSize = commandsSize;
}

const char * menuExecuteCommand(char * const commandNameWithArg) {
    strTrim(commandNameWithArg);
    const int32_t sep = commandSepNameFromArg(commandNameWithArg);
    const char *commandArg = NULL;
    if (sep != -1) {
        commandNameWithArg[sep] = '\0';
        commandArg = commandNameWithArg + sep + 1;
    }
    const char * const commandName = commandNameWithArg;
    for (uint16_t i = 0; i < menuSize; i++) {
        const Command * const command = menuCommands[i];
        if (strcmp(commandGetName(menuCommands[i]), commandName) == 0) {
            return commandExecute(command, commandArg);
        }
    }
    return "Command not found, print \"help\" for getting info\n";
}

static uint8_t isSpaceChar(char ch) {
    return ch == '\n' || ch == ' ' || ch == '\t' || ch == '\r';
}

static void strTrim(char * const str) {
    const int32_t len = strlen(str);
    for (int32_t i = len - 1; i >= 0; i--) {
        if (isSpaceChar(str[i])) {
            str[i] = '\0';
        } else {
            return;
        }
    }
}

static int32_t commandSepNameFromArg(const char * const commandNameWithArg) {
    const int32_t len = strlen(commandNameWithArg);
    for (int32_t i = len - 1; i >= 0; i--) {
        if (commandNameWithArg[i] == ' ') {
            return i;
        }
    }
    return -1;
}
