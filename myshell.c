/*
 * Natalie Sandford - 130031439
 */
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define TRUE 1
#define DELIMS " \t\r\n"

/*
	 *	Detect and report errors (e.g. outputting an error message if the user
	 *		enters an incorrect command - see errno, perror, strerror etc).
	 *	Detect and continue if no command is entered (e.g. the user just enters
	 *		return at prompt).
	 *	Provide a command for the user to exit the shell (e.g. exiting if the
	 *		user enters q at prompt).
	 *	Other enhancements of your own that make the shell more usable, e.g.:
			*	providing built-in commands	
			*	providing user help messages
			*	allowing a user to specify the string used for the command prompt
			*	accessing the user enviroment for program paths, etc.
*/

void menu();
void prompt();
int readcmd();
void parse(char *line);
void fork_exec();
void execute();
void edit_prompt();

char *prompt_str = "->"; /*  */
char *cmd;
char *params[] = { NULL };
char *envp[] = { NULL };
int stat;

size_t linecap = 0;
ssize_t linelen;

/* 
 * Main function.
 * Calls menu function at start of program.
 * Loops forever to take user input and execute commands.
 */
int main()
{
	menu();
	
	while (TRUE) // Repeat forever, until user enters command to exit.
	{
		prompt(); // Display user prompt to enter command.
		if (readcmd() == 0) // If user command is read and returns 0, call the fork_exec function.
		{
			fork_exec();
		}
	}
	return 0;
}

/* 
 * Prints user prompt for input.
 */
void prompt()
{
	printf("%s ", prompt_str);
}

/* 
 * Function to read in the user input.
 * While loop checks user input for:
		* 'q' to exit shell
		* '\n' to ignore command and break loop
		* 'menu' to display the user menu and break loop
		* 'prompt' to allow to user to change the shell prompt and break loop
		* else call parse function to process the user input
 * Returns 0 if command was parsed successfully, else -1 to indicate to the main
	function not to continue.
 */
int readcmd()
{
	char *line = (char *) malloc(linecap + 1); // Dynamically allocate memory.
	
	while ((linelen = getline(&line, &linecap, stdin)) != -1)
	{		
		if (strcmp(line, "q\n") == 0) // Check if string is 'q\n'
		{
			printf("Exiting....\n");
			exit(EXIT_SUCCESS);
			return -1;
		}
		else if (strcmp(line, "\n") == 0) // Check if string is '\n'
		{
			return -1;	
		}
		else if (strcmp(line, "menu\n") == 0) // Check if string is 'menu\n'
		{
			menu();
			return -1;
		}
		else if (strcmp(line, "prompt\n") == 0) // Check if string is 'prompt\n'
		{
			edit_prompt();
			return -1;
		}
		else
		{
			parse(line);
			return 0;
		}
		break;
	}
	return 0;
}

/* 
 * Function to parse the user input to be executed.
 * Input is delimtited into the array params.
 * The first string is passed to the cmd string.
 */
void parse(char *line)
{
	line = strtok(line, DELIMS);
	cmd = line;
	int str_c = 0; // Initialise the string counter to 0 for each new user input.
	
	while (line != NULL)
	{
		params[str_c] = line;
		line = strtok (NULL, DELIMS);
		str_c++;
	}
	params[str_c++] = 0; // Add null terminator to end of array.
}

/* 
 * Function to display a simple help menu to the user.
 */
void menu()
{
	printf("|----------------- Menu ------------------|\n");
	printf("|----- To exit at any time press 'q' -----|\n");
	printf("|------ followed by the return key. ------|\n");
	printf("|-----------------------------------------|\n");
	printf("|--- Enter commands such as '/bin/pwd' ---|\n");
	printf("|-- or '/bin/ls -al' to use the shell. ---|\n");
	printf("|-----------------------------------------|\n");
	printf("|-- To edit the string prompt for this  --|\n");
	printf("|--- shell, enter 'prompt' followed by ---|\n");
	printf("|-- return and follow the instructions. --|\n");
	printf("|-----------------------------------------|\n");
	printf("|-- To view this menu again at any time --|\n");
	printf("|---- type 'menu' followed by return. ----|\n");
	printf("|-------------- End of Menu --------------|\n");
}

/* 
 * Funtion to allow the user to change the prompt used in the shell.
 * Takes user input and then sets that string as the new prompt.
 * ignores a blank returned line.
 * Exits back into main shell if user enters 'q'.
 */
void edit_prompt()
{
	char *new_str = (char *) malloc(linecap + 1);
	
	printf("Enter new prompt, or 'q' to cancel: \n"); // Instruct the user on what to do.
	prompt();
	
	while ((linelen = getline(&new_str, &linecap, stdin)) != -1)
	{
		/* 
		 * Check for 'q' to exit, or for '\n' to ignore.
		 * Else make prompt_str equal user input.
		 */
		if (strcmp(new_str, "q\n") == 0)
		{
			printf("Prompt not changed.\n");
			break;
		}
		else if (strcmp(new_str, "\n") == 0)
		{
			prompt();
			continue;
		}
		else
		{
			prompt_str = strtok(new_str, "\n");
			printf("Prompt saved: %s\n", prompt_str);
			break;
		}
	}
}

/* 
 * Funtion to fork a parent and child process.
 * Calls the execute function within the child process.
 */
void fork_exec()
{
	int pid;
	
	pid = fork();
//	printf("Value of returned pid: %d\n", pid);
	
	if (pid != 0) // In parent process
	{
//		printf("In parent, returned pid: %d\n", pid);
//		printf("Parent about to wait for child...\n");
		
		waitpid(-1, &stat, 0); // Wait for child to finish executing command
		
//		printf("Back in parent - child exited with status: %d\n", WEXITSTATUS(stat));
	}
	else // In child process
	{
//		printf("In child, about to execute %s...\n", cmd);
		
		execute(); // Execute command
	}
}

/* 
 * Function to call the execve function and report errors if the execve failed.
 */
void execute()
{	
	execve(cmd, params, envp);
	
	// If errno != 0, print out error message to user
	if (errno)
	{
		perror("Failed to execute command");	
	}
	
	exit(0);
}