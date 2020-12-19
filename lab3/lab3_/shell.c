#include "shell.h"

// shell commands
const char shellCommands[NUM_COMMANDS][MAX_COMMAND_NAME_SIZE] = {"exit\0", "cd\0", "pwd\0", "wait\0", "ls\0", "py\0", "echo\0", "comp_run_c\0", "clear\0", "debug\0"};

// checks validity of given command
bool IsValidCommand(char* command)
{
	bool isCommand = false;
	for(int i=0; i < NUM_COMMANDS; i++)
	{
		if (strcmp(command, shellCommands[i]) == 0)
			isCommand = true;
	}
	return isCommand;
}

const char ERROR_MESSAGES[NUM_ERRORS][MAX_ERROR_MSG_SIZE] = {
    "[00] Unknown Error",
    "[01] (Input): File Does Not Exist",
    "[02] (Input): Could Not Read From STDIN",
    "[03] (Input): Max Input Line Length Exceeded",
    "[04] (Syntax): Only One Argument For Redirection Allowed",
    "[05] (Syntax): Expected Redirection File After '>'",
    "[06] (Syntax): Not A Valid Command",
    "[07] (Syntax): Too Many Arguments",
    "[08] (Execution): Failure In Execution",
    "[09] (Execution): Redirection Not Allowed",
    "[10] (Execution): No Such Directory",
    "[11] (Execution): Process Creation Failed",
    "[12] (Output): Redirection Error",
};

// Error-Handling function
void Error(int errorCode, const char* extra)
{
	// restore stdout just in case
	dup2(save_stdout, STDOUT_FILENO);

    if (extra)  printf("ERROR! %s:\n\t%s\n", ERROR_MESSAGES[errorCode], extra);
    else        printf("ERROR! %s\n", ERROR_MESSAGES[errorCode]);
}