/**********************************************************************
 * Copyright (c) 2020
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#define _CRT_SECURE_NO_WANINGS
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "types.h"
#include "parser.h"

/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING FROM THIS LINE ******       */
/**
 * String used as the prompt (see @main()). You may change this to
 * change the prompt */
static char __prompt[MAX_TOKEN_LEN] = "$";

/**
 * Time out value. It's OK to read this value, but ** SHOULD NOT CHANGE
 * IT DIRECTLY **. Instead, use @set_timeout() function below.
 */
static unsigned int __timeout = 2;

static void set_timeout(unsigned int timeout)
{
	__timeout = timeout;

	if (__timeout == 0) {
		fprintf(stderr, "Timeout is disabled\n");
	} else {
		fprintf(stderr, "Timeout is set to %d second%s\n",
				__timeout,
				__timeout >= 2 ? "s" : "");
	}
}
/*          ****** DO NOT MODIFY ANYTHING UP TO THIS LINE ******      */
/*====================================================================*/


/***********************************************************************
 * run_command()
 *
 * DESCRIPTION
 *   Implement the specified shell features here using the parsed
 *   command tokens.
 *
 * RETURN VALUE
 *   Return 1 on successful command execution
 *   Return 0 when user inputs "exit"
 *   Return <0 on error
 */

char name[MAX_TOKEN_LEN];
pid_t pid = 0;
int timeout = 0;

void timeout_handler (int sig)
{
	printf("죽엇다\n");
	printf("%s is timed out\n", name);
	kill(pid,SIGKILL);
}

struct sigaction sa;
 //= {
//sa.sa_handler = signal_handler;
//.sa_flags = 0,
//}, old_sa;

static int run_command(int nr_tokens, char *tokens[])
{

				
	/* This 
	function is all yours. Good luck! */

	if (strncmp(tokens[0], "exit", strlen("exit")) == 0) {
		return 0;
	}

	else if(strcmp(tokens[0], "prompt")==0)
	{
		strcpy(__prompt, tokens[1]);
	}

	else if(strcmp(tokens[0], "cd")==0)
	{
		char *hdir = getenv("HOME");
		
		char pdir[MAX_TOKEN_LEN];

		if(strcmp(tokens[1], "~")==0){	
		chdir(hdir);
		}
		else {
		chdir(tokens[1]);
		}
	}

	else if(strcmp(tokens[0], "for")==0)
	{
		int lnum = atoi(tokens[1]);

		if(strcmp(tokens[3], "for")==0) 
        {   int i = 0; 
            for(int j = 0; j < nr_tokens; j++)
            { 
                strcpy(tokens[i],tokens[i+2]); 
            }
			nr_tokens-=2;
            for(int f = 0; f <lnum; f++)
			{ run_command(nr_tokens, tokens+i); }
        }
        else { for(int f = 0; f <lnum; f++) { run_command(nr_tokens , tokens+2);} }
	}

	else if(strcmp(tokens[0], "timeout")==0)
	{
		if(nr_tokens == 1){ 
			printf("Current timeout is %u seconds\n", __timeout);
		}
		else{ 
			timeout = atoi(tokens[1]);
			set_timeout(timeout);
		}
	}

	else {
			strcpy(name, tokens[0]);
	sa.sa_flags = 0;
		int status;
		//fflush(stdin);
		pid = fork();
			if(pid)
			{
				sa.sa_handler = timeout_handler;
				alarm(__timeout);
				sigaction(14, &sa, NULL);
				wait(&status);
				alarm(0);
			}
			else {
				if(execvp(tokens[0],tokens) < 0) 
				{
	  				fprintf(stderr, "No such file or directory\n");
					exit(0);  
				}
			}	
	}
	return 1;
}


/***********************************************************************
 * initialize()
 *
 * DESCRIPTION
 *   Call-back function for your own initialization code. It is OK to
 *   leave blank if you don't need any initialization.
 *
 * RETURN VALUE
 *   Return 0 on successful initialization.
 *   Return other value on error, which leads the program to exit.
 */
static int initialize(int argc, char * const argv[])
{
	return 0;
}


/***********************************************************************
 * finalize()
 *
 * DESCRIPTION
 *   Callback function for finalizing your code. Like @initialize(),
 *   you may leave this function blank.
 */
static void finalize(int argc, char * const argv[])
{

}


/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING BELOW THIS LINE ******      */

static bool __verbose = true;
static char *__color_start = "[0;31;40m";
static char *__color_end = "[0m";

/***********************************************************************
 * main() of this program.
 */
int main(int argc, char * const argv[])
{
	char command[MAX_COMMAND_LEN] = { '\0' };
	int ret = 0;
	int opt;

	while ((opt = getopt(argc, argv, "qm")) != -1) {
		switch (opt) {
		case 'q':
			__verbose = false;
			break;
		case 'm':
			__color_start = __color_end = "\0";
			break;
		}
	}

	if ((ret = initialize(argc, argv))) return EXIT_FAILURE;

	if (__verbose)
		fprintf(stderr, "%s%s%s ", __color_start, __prompt, __color_end);

	while (fgets(command, sizeof(command), stdin)) {	
		char *tokens[MAX_NR_TOKENS] = { NULL };
		int nr_tokens = 0;

		if (parse_command(command, &nr_tokens, tokens) == 0)
			goto more; /* You may use nested if-than-else, however .. */

		ret = run_command(nr_tokens, tokens);
		if (ret == 0) {
			break;
		} else if (ret < 0) {
			fprintf(stderr, "Error in run_command: %d\n", ret);
		}

more:
		if (__verbose)
			fprintf(stderr, "%s%s%s ", __color_start, __prompt, __color_end);
	}

	finalize(argc, argv);

	return EXIT_SUCCESS;
}
