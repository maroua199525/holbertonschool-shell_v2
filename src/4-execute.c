#include "shell.h"
#include <sys/wait.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>

/**
 * execute_line - makes linked list of commands from tokens
 * @tokens: tokens array
 * Return: cmd of command_t list
 */
void execute_line(char **tokens)
{
	command_t *cmd = NULL;
	int stdin_cpy = dup(STDIN_FILENO), stdout_cpy = dup(STDOUT_FILENO);
	int prev_logic = 0, i;

	/* Loop through all tokens while shell is running */
	for (i = 0; tokens[i] && shell.run; i++)
	{
		/* MAKE COMMAND */
		if (IS_SEPARATOR(tokens[i]) || IS_REDIR_TOKEN(tokens[i]))
		{
			shell.status = handle_syntax_error(tokens[i--]);
			break;
		}
		cmd = make_command(tokens, &i);
		if (get_IO(cmd, prev_logic) == 1)
		{
			/* Set input and output */
			dup2(cmd->input_fd, STDIN_FILENO);
			dup2(cmd->output_fd, STDOUT_FILENO);

			/* Execute command (or handle error) */
			if (cmd->executor)
				shell.status = cmd->executor(cmd->args);
			else if (cmd->path)
				shell.status = fork_and_exec(cmd);
			else
				shell.status = handle_error(shell.status, cmd->command);

			/* Reset input and output */
			dup2(stdin_cpy, STDIN_FILENO);
			dup2(stdout_cpy, STDOUT_FILENO);
		}
		prev_logic = cmd->logic;
		if (clean_pipes(cmd) == 0)
			break;
	}
	while (tokens[++i])
		free(tokens[i]);
}

/**
 * make_command - makes a single command
 * @tokens: array of arguments
 * @i: index of token array we left off at
 * Return: command to execute
 */
command_t *make_command(char **tokens, int *i)
{
	int j;
	command_t *cmd = command_config(replace_vars(tokens[(*i)++]));

	/* COMMAND READING LOOP (ENDS WHEN THE COMMANDS ENDS*/
	for (j = 1; !IS_SEPARATOR(tokens[*i]); (*i)++)
	{
		/* Detect file redirectors */
		if (tokens[*i][0] == '<')
		{
			cmd->logic |= (tokens[*i][1] == '<') ? IS_HEREDOC : IS_REDIR_IN;
			free(tokens[(*i)++]), cmd->input = replace_vars(tokens[*i]);
		}
		else if (tokens[*i][0] == '>')
		{
			cmd->logic |= (tokens[*i][1] == '>') ? IS_APPEND : IS_REDIR_OUT;
			free(tokens[(*i)++]), cmd->output = replace_vars(tokens[*i]);
		}
		else if (IS_NUMERIC(tokens[*i][0]) && tokens[*i][1] == '>')
		{
			cmd->logic |= HAS_EXTRA;
			free(tokens[(*i)++]), cmd->output = replace_vars(tokens[*i]);
		}
		else /* if not a redirect, save to argument array */
		{
			cmd->args[j++] = replace_vars(tokens[*i]);
		}
	}

	/* COMMAND SEPARATOR LOGIC */
	if (tokens[*i])
	{
		if (tokens[*i][0] == '|')
			cmd->logic |= (tokens[*i][1] == '|') ? IS_OR : IS_PIPE;
		else if (_strcmp(tokens[*i], "&&") == 0)
			cmd->logic |= IS_AND;
		free(tokens[(*i)]);
	}

	return (cmd);
}

/**
 * replace_vars - detects an replaces variables in a shell token
 * @token: token
 * Return: token with all variables replaces
 **/
char *replace_vars(char *token)
{
	char *new_token, *value;
	int i;

	/* check for a '$' . If no dollar signs, return token */
	for (i = 0; token[i] != '$'; i++)
		if (token[i] == '\0')
			return (token);

	if (!token[i + 1] || token[i + 1] == ' ')
		return (token);

	value = _realloc(NULL, sizeof(char) * 12);

	if (_strcmp(token + i, "$$") == 0)
		sprintf(value, "%d", getpid());
	else if (_strcmp(token + i, "$?") == 0)
		sprintf(value, "%d", shell.status);
	else
		free(value), value = _getenv(token + i + 1);

	/* Create token */
	new_token = _realloc(NULL, i + _strlen(value) + 1);
	_strncpy(new_token, token, i);
	_strcat(new_token, value);
	free(token);
	free(value);

	return (replace_vars(new_token)); /* check for more variables */
}

/**
 * clean_pipes - clean pipes
 * @cmd: cmd
 * Return: to keep running shell or not
 **/
int clean_pipes(command_t *cmd)
{
	int logic = cmd->logic, i;

	if (cmd->input_fd > 2)
		close(cmd->input_fd);
	if (cmd->output_fd > 2)
		close(cmd->output_fd);

	free(cmd->path);
	free(cmd->input);
	free(cmd->output);
	if (cmd->args)
		for (i = 0; cmd->args[i]; i++)
			free(cmd->args[i]);
	free(cmd->args);
	free(cmd);

	if (shell.status && (logic & IS_AND))
		return (false);
	if (!shell.status && (logic & IS_OR))
		return (false);

	return (true);
}

/**
 * fork_and_exec - forks and executes
 * @cmd: command node
 * Return: exit status of execution
 **/
int fork_and_exec(command_t *cmd)
{
	pid_t child_pid;
	int status = 0;

	child_pid = fork();

	/* child executes */
	if (child_pid == 0)
	{
		shell.status = execve(cmd->path, cmd->args, environ);
		exit(handle_error(errno, cmd->command));
	}

	/* Parent waits and returns status */
	if (child_pid == -1 || waitpid(child_pid, &status, 0) == -1)
		return (handle_error(errno, cmd->command));
	else
		return (WEXITSTATUS(status));
}
