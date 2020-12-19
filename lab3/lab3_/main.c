#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "shell.h"

bool runShell = true;
bool batchMode = false;
bool debugMode = false;
int save_stdout;

char inp[MAX_INP_SIZE+1] = "\0";

int usage(char* prog)
{
    fprintf(stderr, "usage: %s [optional:batchFile] <-d bool: show input info>\n", prog);
    exit(1);
}

int main(int argc, char* argv[])
{
	// allowed usage:	mysh [batchFile]
	if (argc > 3) usage(argv[0]);

	// check if to show input info
	int c;
	while((c = getopt(argc, argv, "d")) != -1)
    {
        switch(c)
        {
            case 'd':
                debugMode = true;
                break;
		}
	}

	// save STDOUT FD
	save_stdout = dup(STDOUT_FILENO);

	// for batch-mode
	FILE* fp = NULL;

	// check for batch mode
	if (argc >= 2)
	{
		// input file path
	    char* batchFile = strdup(argv[1]);

	    // open input file
		if(strchr(batchFile, '-') != NULL)
		{
			if (argc == 3)
			{
				batchFile = strdup(argv[2]);
				batchMode = true;
			}
		}
		else 
		{
			batchMode = true;
		}
		

		// if in batch-mode, try to open batch-file
		if (batchMode && (fp = fopen(batchFile, "r+")) == NULL)
		{
			// upon error (file not found)
			Error(ERR_INPUT__FILE_NOT_FOUND, batchFile);
			exit(1);
		}
	}

	// main shell loop
	while(runShell)
	{
		// clear input & print shell name
		strcpy(inp, "");
		if (!batchMode) printf("mysh>");

		// read user-input into 'inp'
		if(batchMode && (fgets(inp, MAX_INP_SIZE, fp) == NULL))
		{
			// EOF
			(void) fclose(fp);
			break;
			/*or */ 	
			//	batchMode = false;	continue;
		}
		else if(!batchMode && (fgets(inp, MAX_INP_SIZE, stdin) == NULL))
		{
			// STDIN Read Error
			if(feof(stdin)) continue;
		    else Error(ERR_INPUT__STDIN_READ, NULL);
		}
		else if (strchr(inp, '\n') == NULL)
		{
			int c;
			while((c = getc(stdin)) != '\n' && c != EOF);
			Error(ERR_INPUT__LIMIT_EXCEEDED, inp);
		}

		// handle empty input
		if (strcmp(inp, "\n") == 0 || strlen(inp) == 0)
		{
			continue;
		}

		// clear new-line
		if (inp[strlen(inp) - 1] == '\n')
		{
			inp[strlen(inp) - 1] = '\0';
		}

		char* orig_inp = strdup(inp);

		// Handle Python and C files

		if(IsPythonScript(strdup(inp)))
		{
			char** name = malloc(sizeof(char*));
			strtok(inp, ".");
			name[0] = inp;
			Execute(shellCommands[COMM__py], 0, name, NULL);
			free(name);
			continue;
		}

		if(IsCFile(strdup(inp)))
		{
			char** name = malloc(sizeof(char*));
			strtok(inp, ".");
			name[0] = inp;
			Execute(shellCommands[COMM__comp_run_c], 0, name, NULL);
			free(name);
			continue;
		}

		// parse input into command, arguments & redirection file
		
		char* delim = ">";
	    char* commArg = strtok(inp, delim);
	    char* redirArg = strtok(NULL, delim);

	    // check for redirection file
	    if (redirArg != NULL)
	    {
	    	if (strtok(NULL, delim) != NULL)	{ Error(ERR_SYN__REDIR_TOO_MANY_ARGS, NULL); continue; }
	    	delim = " ";
	    	redirArg = strtok(redirArg, delim);
	    	if (strtok(NULL, delim) != NULL)	{ Error(ERR_SYN__REDIR_TOO_MANY_ARGS, NULL); continue; }
	    }
	    else if(strchr(orig_inp, '>') != NULL)
	    {
	    	Error(ERR_SYN__NO_REDIR_FILE, NULL);
			continue;
		}

		delim = " ";
		char* command = strtok(strdup(commArg), delim);

		if(!IsValidCommand(command))
		{
			Error(ERR_SYN__INVALID_COMMAND, command);
			continue;
		}

		char* firstArg = strtok(NULL, delim);

		if(firstArg == NULL)
		{
			// no arguments
			Execute(command, 0, NULL, redirArg);
		}
		else
		{
			// as a safe measure, allocate using commArg string size
			char** args = (char**) malloc(sizeof(char*) * strlen(commArg));
			args[0] = firstArg;

			int numArgs = 0;
			do
			{
				numArgs++;
				args[numArgs] = strtok(NULL, delim);
			}
			while(args[numArgs] != NULL);

			Execute(command, numArgs, args, redirArg);

			free(args);
		}
	}

	return 0;
}