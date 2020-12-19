#include "shell.h"
#include <string.h>

// check if input is a python-extension file
bool IsPythonScript(char* file)
{
	char* delim = ".";
	strtok(file, delim);
	char* ext = strtok(NULL, delim);
	if(ext == NULL)
		return false;
	if(strcmp(ext, "py") == 0 && strtok(NULL, delim) == NULL)
		return true;
	return false;
}

// check if input is a c-extension file
bool IsCFile(char* file)
{
	char* delim = ".";
	strtok(file, delim);
	char* ext = strtok(NULL, delim);
	if(ext == NULL)
		return false;
	if(strcmp(ext, "c") == 0 && strtok(NULL, delim) == NULL)
		return true;
	return false;
}

// to show command name, arguments, and redirection file name
void InputInfo(const char* command, int numArgs, char** args, char* redirArg)
{
	printf("\tCOMMAND:\t%s\n", command);
	if(numArgs!=0)
	{
		printf("\tARGUMENTS(%d):\t", numArgs);
		for(int i=0; i<numArgs; i++)
		{
			printf("%s", args[i]);
			if(i != numArgs-1) printf(", ");
		}
		printf("\n");
	}
	else printf("\tNO ARGUMENTS\n");
	if(redirArg!=NULL)
	{
		printf("\tREDIRECTION:\t%s\n", redirArg);
	}
	else printf("\tNO REDIRECTION\n");
}