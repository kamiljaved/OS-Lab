#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// shell constants
#define MAX_INP_SIZE 512
#define NUM_COMMANDS 10
#define MAX_COMMAND_NAME_SIZE 15
#define NUM_ERRORS 14
#define MAX_ERROR_MSG_SIZE 100
// shell command constants
#define COMM__exit          0
#define COMM__cd            1
#define COMM__pwd           2
#define COMM__wait          3
#define COMM__ls            4
#define COMM__py            5
#define COMM__echo          6
#define COMM__comp_run_c    7
#define COMM__clear         8
#define COMM__debug         9


// shell error constants
#define ERR_UNKNOWN                     0
#define ERR_INPUT__FILE_NOT_FOUND       1
#define ERR_INPUT__STDIN_READ           2
#define ERR_INPUT__LIMIT_EXCEEDED       3
#define ERR_SYN__REDIR_TOO_MANY_ARGS    4
#define ERR_SYN__NO_REDIR_FILE          5
#define ERR_SYN__INVALID_COMMAND        6
#define ERR_SYN__TOO_MANY_ARGS          7
#define ERR_EXEC__FAILURE               8
#define ERR_EXEC__REDIR_NOT_ALLOWED     9
#define ERR_EXEC__NO_SUCH_DIR           10
#define ERR_EXEC__PROC_CREAT_FAILURE    11
#define ERR_OUT__REDIR_FAIL             12

// shell variables
extern bool runShell;
extern bool batchMode;
extern int save_stdout;
extern bool debugMode;
extern char inp[MAX_INP_SIZE+1];
extern const char shellCommands[NUM_COMMANDS][MAX_COMMAND_NAME_SIZE];
extern const char ERROR_MESSAGES[NUM_ERRORS][MAX_ERROR_MSG_SIZE];

// shell base functions
bool IsValidCommand(char* command);
void Error(int errorCode, const char* extra);

// shell execution functions
void Execute(const char* command, int numArgs, char** args, char* redirArg);
void RunChildProcess(char* argv[]);

// shell helper functions
bool IsPythonScript(char* file);
bool IsCFile(char* file);
void InputInfo(const char* command, int numArgs, char** args, char* redirArg);



////////////////////////////////////////////////////////////////////////////////////////
//                                      help text                                     //
////////////////////////////////////////////////////////////////////////////////////////

// strchr
// const char * strchr ( const char * str, int character );
// Returns a pointer to the first occurrence of character in the C string str.

// execvp
// int execvp(const char *file, char *const argv[]);
// The first argument is the file you wish to execute, and the second argument is an array of null-terminated strings that represent the appropriate arguments to the file.
// The exec() functions only return if an error has occurred. The return value is -1, and errno is set to indicate the error.