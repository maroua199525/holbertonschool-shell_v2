#include "../shell.h"
#include <unistd.h>
#include <stdio.h>

/**
 * builtin_exit - custom exit built-in
 * @args: arguments (string array)
 * Return: exit status
 **/
int builtin_exit(char **args)
{
	int status = 0, i = 0;
	char error[256], *str = "%s: %d: %s: %s\n";

	/* Check for numeric exit code */
	if (args[1])
	{
		/* If exit code is invalid, print error */
		for (i = 0; args[1][i]; i++)
			if (!IS_NUMERIC(args[1][i]))
			{
				sprintf(error, str, shell.name, shell.lines, "exit", args[1]);
				write(STDERR_FILENO, error, _strlen(error));
				return (2);
			}

		/* Else, calculate exit code */
		for (i = 0; args[1][i]; i++)
			status = 10 * status + (args[1][i] - '0');
	}
	else
	{
		status = shell.status;
	}

	/* set shell.run = false */
	shell.run = false;

	/* Return exit code */
	return (status);
}

/**
 * help_exit - prints help exit content
 **/
void help_exit(void)
{
	int i;
	char *lines[] = {
		"exit: exit",
		"\tExits the shell.",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}
