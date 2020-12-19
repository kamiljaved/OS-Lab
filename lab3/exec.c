#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
	printf("hello world (pid:%d)\n", (int) getpid());
	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) { // child (new process)
		printf("hello, I am child (pid:%d)\n", (int) getpid());
		char *myargs[3];
		myargs[0] = strdup("/bin/ls");// path find using which ls
		myargs[1] = strdup("-l"); // input arg to ls
		myargs[2] = NULL;// mark end of array

		execvp(myargs[0], myargs); // runs ls
		printf("exec failed");
	} else {
		int wc;
		wc = wait(NULL);
		printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",rc, wc, (int) getpid());
}
return 0;
}
