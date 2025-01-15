#include "shell.h"
#include <sys/stat.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>

/**
 * command_config - inits new command node
 * @command: command that the node represent
 * Return: New node
 */
command_t *command_config(char *command)
{
	int i;
	command_t *new = malloc(sizeof(command_t));

	if (!new)
		return (NULL);

	command = get_alias(command);
	new->logic = DEFAULT_LOGIC;
	new->command = command;
	new->path = get_program_path(command);
	new->executor = get_executor(command);
	new->input = NULL;
	new->output = NULL;
	new->input_fd = STDIN_FILENO;
	new->output_fd = STDOUT_FILENO;
	new->args = malloc(sizeof(char *) * 256);
	new->args[0] = command;
	for (i = 1; i < 256; i++)
		new->args[i] = NULL;
	return (new);
}

/**
 * get_executor - returns pointer to appropriate executor function
 * @command: command (as a string)
 * Return: function pointer
 **/
exec_f get_executor(char *command)
{

	if (_strcmp("cd", command) == 0)
		return (&builtin_cd);
	else if (_strcmp("help", command) == 0)
		return (&builtin_help);
	else if (_strcmp("env", command) == 0)
		return (&builtin_env);
	else if (_strcmp("setenv", command) == 0)
		return (&builtin_setenv);
	else if (_strcmp("history", command) == 0)
		return (&builtin_history);
	else if (_strcmp("unsetenv", command) == 0)
		return (&builtin_unsetenv);
	else if (_strcmp("alias", command) == 0)
		return (&builtin_alias);
	else if (_strcmp("exit", command) == 0)
		return (&builtin_exit);

	return (NULL);
}

/**
 * get_program_path - finds a program in the PATH
 * @program: program name (char *)
 * Return: full program path || just program if not found in path
 **/
char *get_program_path(char *program)
{
	int i;
	struct stat jeffrey;
	char *PATH, *path = NULL, *buffer = NULL;
	char *builtins[] = {"cd", "help", "alias", "setenv", "unsetenv", "exit",
						"history", "env", NULL};

	/* If program is a path, return copy of program */
	for (i = 0; program[i]; i++)
		if (program[i] == '/')
			return (_strdup(program));

	/* If program is builtin or, return copy of program */
	for (i = 0; builtins[i]; i++)
		if (_strcmp(builtins[i], program) == 0)
			return (_strdup(program));

	PATH = _getenv("PATH");
	buffer = malloc(sizeof(char) * 256);

	/* check the PATH */
	while ((path = strtok((path ? NULL : PATH), ":")))
	{
		/* concat path with slash and program name */
		sprintf(buffer, "%s/%s", path, program);
		if (stat(buffer, &jeffrey) == 0)
		{
			if (access(buffer, X_OK) != 0)
			{
				shell.status = EACCES;
				free(PATH), free(buffer);
				return (NULL);
			}
			free(PATH);
			return (buffer);
		}
	}
	shell.status = ENOENT;
	free(PATH);
	free(buffer);
	return (NULL);
}
