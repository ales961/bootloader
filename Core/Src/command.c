#include "command.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct Command {
    char *name;
    CommandAction action;
    CommandArgument argType;
};

static uint8_t parseInt(const char *str, uint32_t *result);
static uint8_t parseOnOffStatus(const char *str, OnOffStatus *result);

Command * commandCreate(const char * const name, const CommandAction action, const CommandArgument argType) {
    size_t nameLen = strlen(name);
    char *nameCopy = (char *) malloc(sizeof(char) * (nameLen + 1));
    strcpy(nameCopy, name);
    Command *command = (Command *) malloc(sizeof(Command));
    command->name = nameCopy;
    command->action = action;
    command->argType = argType;
    return command;
}

const char * commandExecute(const Command * const command, const char * const argument) {
    if (command == NULL || command->action == NULL)
        return NULL;
    switch (command->argType) {
        case INT:
        {
            uint32_t intArg;
            if (!parseInt(argument, &intArg))
                return "The argument must be integer\n";
            return (*command->action)(&intArg);
        }
        case ON_OFF:
        {
            OnOffStatus statusArg;
            if (!parseOnOffStatus(argument, &statusArg))
                return "The argument must be 'on' or 'off'\n";
            return (*command->action)(&statusArg);
        }
        case NONE:
            return (*command->action)(NULL);
        default:
            return "Unknown argument type\n";
    }
}

static uint8_t parseInt(const char * const str, uint32_t * const result) {
    char *endPtr = NULL;
    *result = strtoul(str, &(endPtr), 10);
    if (endPtr == str) {
        return 0;
    }
    return 1;
}

static uint8_t parseOnOffStatus(const char *str, OnOffStatus * const result) {
    if (strcmp(str, "on") == 0) {
        *result = ON;
        return 1;
    } else if (strcmp(str, "off") == 0) {
        *result = OFF;
        return 1;
    }
    return 0;
}

void commandSetAction(Command * const command, const CommandAction action) {
    if (command == NULL)
        return;
    command->action = action;
}

const char * commandGetName(const Command * const command) {
    if (command == NULL)
        return NULL;
    return command->name;
}

CommandAction commandGetAction(const Command * const command) {
    return command->action;
}
