#include "../shell.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>

/**
 * builtin_setenv - custom setenv built-in. sets new environment variable
 * @args: arguments (string array)
 * Return: exit status
 **/
int builtin_setenv(char **args)
{
	int i;
	char *ENV_VAR;
	char *VARIABLE, *VALUE, *var = NULL;

	/* if only "setenv" passed in, call env */
	if (args[1] == NULL || args[2] == NULL)
	{
		write(STDERR_FILENO, "Usage: setenv VARIABLE VALUE\n", 29);
		return (1);
	}

	/* retrieve our env variable and new corresponding value */
	VARIABLE = args[1];
	VALUE = args[2];

	/* construct env var */
	ENV_VAR = malloc(sizeof(char) * 256);
	sprintf(ENV_VAR, "%s=%s", VARIABLE, VALUE);

	/* loop through enviornment variables */
	for (i = 0; environ[i]; i++)
	{
		var = _strdup(environ[i]);
		if (_strcmp(strtok(var, "="), VARIABLE) == 0)
		{
			free(var), free(environ[i]);
			environ[i] = ENV_VAR;
			return (EXIT_SUCCESS);
		}
		free(var);
	}
	/* set environ [i] to our new env var if it's NULL */
	environ = _realloc_string_array(environ, 1);
	environ[i] = ENV_VAR;
	return (EXIT_SUCCESS);
}

/**
 * help_setenv - prints setenv help content
 **/
void help_setenv(void)
{
	int i;
	char *lines[] = {
		"setenv: setenv [VARIABLE] [VALUE]",
		"\tUpdate, or set a new enviornment variable.",
		"\tIf called with no arguements, a list of all",
		"\tenviornment variables will be printed, see env.",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}
