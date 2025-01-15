#include "../shell.h"
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>

/**
 * handle_error - prints error messages
 * @code: error code
 * @program: program that failed
 * Return: error code
 **/
int handle_error(int code, char *program)
{
	char error[256], *msg;

	if (code == ENOENT)
		msg = "not found", code = 127;
	else if (code == EACCES)
		msg = "Permission denied", code = 126;
	else
		msg = "Unknown error. Inspect exit status.";

	sprintf(error, "%s: %d: %s: %s\n", shell.name, shell.lines, program, msg);
	write(STDERR_FILENO, error, _strlen(error));
	return (code);
}

/**
 * handle_syntax_error - function wrapper for syntax error handling
 * @token: culprit token
 * Return: status code
 **/
int handle_syntax_error(char *token)
{
	char error[256], tmp[8];
	char *str = "%s: %d: Syntax error: %s unexpected\n";

	if (IS_SEPARATOR(token))
		sprintf(tmp, "\"%s\"", token);
	else
		_strcpy(tmp, "newline");

	sprintf(error, str, shell.name, shell.lines, tmp);
	write(STDERR_FILENO, error, _strlen(error));
	return (2);
}
