#include "shell.h"
#include "error.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

bool debugMode;

// function to execute user's commands
void Execute(const char* command, int numArgs, char** args, char* redirArg)
{
	if(debugMode) InputInfo(command, numArgs, args, redirArg);
	
	int fd;
	bool redir = false;
	if(redirArg != NULL) redir = true;

	if (redir)
	{
		close(STDOUT_FILENO);
	    // open/create output file
	    fd = open(redirArg, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	    if (fd < 0)
	    {
	    	// open error
	    	Error(ERR_OUT__REDIR_FAIL, redirArg);
	    	return;
	    }
	}

	if(strcmp(command, shellCommands[COMM__exit]) == 0)				// exit
	{
		if (numArgs!=0)		{ Error(ERR_SYN__TOO_MANY_ARGS, command); return; }
		if (redirArg!=NULL)	{ Error(ERR_EXEC__REDIR_NOT_ALLOWED, redirArg); return; }

		runShell = false;
	}
	else if(strcmp(command, shellCommands[COMM__cd]) == 0) 			// cd
	{
		if (redirArg!=NULL)	{ Error(ERR_EXEC__REDIR_NOT_ALLOWED, redirArg); return; }

		if (numArgs==0)
		{
			int ch = chdir(getenv("HOME"));
		}
		else if (numArgs==1)
		{
			if (chdir(args[0]) != 0)
			{
				Error(ERR_EXEC__NO_SUCH_DIR, args[0]);
				return;
			}
		}
		else
		{
			// Too Many Arguments
			Error(ERR_SYN__TOO_MANY_ARGS, command);
			return;
		}
	}
	else if(strcmp(command, shellCommands[COMM__pwd]) == 0)			// pwd
	{
		if (numArgs!=0)	{ Error(ERR_SYN__TOO_MANY_ARGS, command); return; }

		char* cwd = malloc(sizeof(char)*513);
		int wr = 0;
		if (getcwd(cwd, sizeof(char)*512) != NULL)
		{
			sprintf(cwd + strlen(cwd), "\n");
			wr = write(STDOUT_FILENO, cwd, strlen(cwd));
		}
		else
		{
		   Error(ERR_EXEC__FAILURE, NULL);
		   return;
		}
		free(cwd);
	}
	else if(strcmp(command, shellCommands[COMM__wait]) == 0)		// wait
	{
		if (numArgs!=0)	{ Error(ERR_SYN__TOO_MANY_ARGS, command); return; }
		wait(NULL);
	}
	else if(strcmp(command, shellCommands[COMM__ls]) == 0)			// ls
	{
		char** ls_args = malloc(sizeof(char*) * (numArgs+2));
		ls_args[0] = strdup("/bin/ls");
		int i = 1;
		for(; i <= numArgs; i++)
		{
			ls_args[i] = strdup(args[i-1]);
		}
		ls_args[i] = NULL;
		RunChildProcess(ls_args);
		free(ls_args);
	}
	else if(strcmp(command, shellCommands[COMM__py]) == 0)			// py (python script)
	{
		char* filename = args[0];
		sprintf(filename + strlen(filename), ".py");
		printf("Running Python Script: %s\n", filename);
		char* py_args[3];
		py_args[0] = strdup("python3");
		py_args[1] = strdup(filename);
		py_args[2] = NULL;
		RunChildProcess(py_args);
	}
	else if(strcmp(command, shellCommands[COMM__echo]) == 0)		// echo
	{
		char** echo_args = malloc(sizeof(char*) * (numArgs+2));
		echo_args[0] = strdup("/bin/echo");
		int i = 1;
		for(; i <= numArgs; i++)
		{
			echo_args[i] = strdup(args[i-1]);
		}
		echo_args[i] = NULL;
		RunChildProcess(echo_args);
		free(echo_args);
	}
	else if(strcmp(command, shellCommands[COMM__comp_run_c]) == 0)	// comp_run_c (c file)
	{
		char* filename = args[0];
		char* outputfile = strdup(filename);
		sprintf(filename + strlen(filename), ".c");
		printf("Compiling C File: %s\n", filename);
		char* gcc_args[5];
		gcc_args[0] = strdup("gcc");
		gcc_args[1] = strdup(filename);
		gcc_args[2] = strdup("-o");
		gcc_args[3] = strdup(outputfile);
		gcc_args[4] = NULL;
		RunChildProcess(gcc_args);
		printf("Running C Program: %s\n", outputfile);
		char* cprog_args[2];
		char prog[110];
		if(strlen(outputfile)<=100)
			sprintf(prog + strlen(prog) - 1, "./%s", outputfile);
		else strcpy(prog, "cprogram");
		cprog_args[0] = strdup(prog);
		cprog_args[1] = NULL;
		RunChildProcess(cprog_args);
	}
	else if(strcmp(command, shellCommands[COMM__clear]) == 0)			// clear
	{
		if (numArgs!=0)		{ Error(ERR_SYN__TOO_MANY_ARGS, command); return; }
		if (redirArg!=NULL)	{ Error(ERR_EXEC__REDIR_NOT_ALLOWED, redirArg); return; }

		char** clr_args = malloc(sizeof(char*) * (numArgs+2));
		clr_args[0] = strdup("/bin/clear");
		clr_args[1] = NULL;
		RunChildProcess(clr_args);
		free(clr_args);
	}
	else if(strcmp(command, shellCommands[COMM__debug]) == 0)			// debug
	{
		if (numArgs!=0)		{ Error(ERR_SYN__TOO_MANY_ARGS, command); return; }
		if (redirArg!=NULL)	{ Error(ERR_EXEC__REDIR_NOT_ALLOWED, redirArg); return; }

		debugMode = !debugMode;
		if (debugMode) printf("\tDEBUG MODE [ON]\n");
		else printf("\tDEBUG MODE [OFF]\n");
	}

	// restore STDOUT FD
	if(redir)
	{
		dup2(save_stdout, STDOUT_FILENO);
	}
}

// creates a child process to run specific commands with arguments
void RunChildProcess(char* argv[])
{
	int rc = fork();
	if (rc < 0)
	{
		Error(ERR_EXEC__PROC_CREAT_FAILURE, NULL);
		return;
	}
	else if (rc == 0)
	{
		// printf("child process (pid:%d)\n", (int) getpid());
		execvp(argv[0], argv);
		// following line(s) should not be executed upon successful exec
		Error(ERR_EXEC__PROC_CREAT_FAILURE, NULL);
	}
	else
	{
		// in parent: rc is child-PID
		// printf("parent process (pid:%d)\n", (int) getpid());
		wait(NULL);
	}

}
