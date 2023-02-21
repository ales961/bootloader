#ifndef COMAND_H
#define COMAND_H

typedef struct Command Command;

typedef const char * (*CommandAction)(const void *);

typedef enum OnOffStatus { ON, OFF } OnOffStatus;
typedef enum CommandArgument { INT, ON_OFF, NONE } CommandArgument;

Command * commandCreate(const char *name, CommandAction action, CommandArgument argType);
const char * commandExecute(const Command *command, const char *argument);
void commandSetAction(Command *command, CommandAction action);
const char * commandGetName(const Command *command);
CommandAction commandGetAction(const Command * command);


#endif //COMAND_H
