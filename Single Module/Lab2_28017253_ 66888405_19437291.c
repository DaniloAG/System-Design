/*
	Gonzalez, Danilo: 28017253
	Orozco, Jonathan: 66888405
	Almazan, Adrian: 19437291

	IMPORANT!! 
	We copied 3 functions from the textbook because Professor Harris
	said it was okay to do so. We simply had to figure out a way to
	reap the background process once the shell was terminated. So,
	some of the functions are modified to meet the projects requirement.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAXARGS 128
#define	MAXLINE	 8192

extern char **environ;
/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
void reapCommands();
void reaper();

int globalCounter = 0;
pid_t toReap[MAXARGS];

int globalCompleted = 0;
pid_t completedReaps[MAXARGS];

int main(){
	char cmdline[MAXLINE]; 
	while (1) {
		printf("prompt> ");
 		fgets(cmdline, MAXLINE, stdin);
 		if (feof(stdin)){
 			exit(0);
 		}
 		eval(cmdline);
	}
}

void eval(char *cmdline){
	char *argv[MAXARGS]; 				/* Argument list execve() */
	char buf[MAXLINE]; 					/* Holds modified command line */
	int bg; 							/* Should the job run in bg or fg? */
	pid_t pid; 							/* Process id */
	extern int globalCounter;
	extern pid_t toReap[];

	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return; 						/* Ignore empty lines */
	// printf("%d\n", !builtin_command(argv));
	if (!builtin_command(argv)) {
		if ((pid = fork()) == 0) { 		/* Child runs user job */
			if (execve(argv[0], argv, environ) < 0) {
	 			printf("%s: Command not found.\n", argv[0]);
	 			exit(0);
	 		}
 		}
 		else{
 			if (bg){
 				toReap[globalCounter] = pid;
				globalCounter++;
 			}
 		}
 	/* Parent waits for foreground job to terminate */
 		if (bg){
 			printf("%d %s", pid, cmdline);
 			signal(SIGCHLD, reaper);
 		}
 		else{
 			int status;
 			pid_t wpid;
 			do {
				printf("Waiting for child process to terminate \n");
				wpid = waitpid(pid, &status, 0);
			} while( !WIFEXITED(status));
 			// int status;
 			// if (waitpid(pid, &status, 0) < 0){
 			// 	printf("done processing\n");
 			// 	fprintf(stderr, "%s: %s\n", "waitfg: waitpid error", strerror(errno));
    // 			exit(0);
 			// }
 		}
 	}
 	return;
 }

 /* If first arg is a builtin command, run it and return true */
 int builtin_command(char **argv)
 {
 	if (!strcmp(argv[0], "quit")){ 		/* quit command */
 		// printf("quitting\n");
 		reapCommands();
 		exit(0);
 	}
 	if (!strcmp(argv[0], "&")) 			/* Ignore singleton & */
 		return 1;
 	
 	return 0;							 /* Not a builtin command */
 }


 /* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv)
{
	char *delim; 					/* Points to first space delimiter */
	int argc; 						/* Number of args */
	int bg; 						/* Background job? */

 	buf[strlen(buf)-1] = ' ' ; 		/* Replace trailing ’\n’ with space */
 	while (*buf && (*buf == ' ' )) 	/* Ignore leading spaces */
 		buf++;

 	/* Build the argv list */
 	argc = 0;
 	while ((delim = strchr(buf, ' '))) {
 		argv[argc++] = buf;
 		*delim = '\0';
 		buf = delim + 1;
 		while (*buf && (*buf == ' ')) 		/* Ignore spaces */
 			buf++;
 	}
 	argv[argc] = NULL;

	if (argc == 0)						 /* Ignore blank line */
		return 1;

 	/* Should the job run in the background? */
	if ((bg = (*argv[argc-1] == '&')) != 0)
 		argv[--argc] = NULL;

	return bg;
}

void reaper(){
	pid_t process;
	int status;
	while ((process = waitpid(-1, &status, WNOHANG)) > 0){
		
	}
}

void reapCommands(){
	extern int globalCounter;
	extern pid_t toReap[];
	int temp;
	int i;
	for (i = 0; i < globalCounter; i++){
		printf("Killing process %d\n", toReap[i]);
		kill(toReap[i], SIGINT);
	}
	for (i = 0; i < globalCounter; i++){
		pid_t wpid = wait(&temp);
		// if (WIFEXITED(temp)) 
		// 	printf("Child %d terminated %d\n", wpid, WEXITSTATUS(temp));
		// else 
		// 	printf("Child terminated abnormally\n");
	}
}