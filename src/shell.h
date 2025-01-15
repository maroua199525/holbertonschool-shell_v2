#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include "strings/my_strings.h"

#define HISTSIZE 4096
#define READ_SIZE 1

#define DEFAULT_LOGIC 0000
#define IS_REDIR_OUT 0001
#define IS_REDIR_IN 0002
#define IS_APPEND 0004
#define IS_HEREDOC 0010
#define IS_AND 0020
#define IS_OR 0040
#define IS_PIPE 0100
#define HAS_EXTRA 0200

#define SYNTAX_ERROR 105
#define CANT_OPEN 127

#define IS_NUMERIC(x) ((x) >= '0' && (x) <= '9')
#define IS_ALPHA(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))
#define IS_SEPARATOR(x) (!x || x[0] == ';' || x[0] == '|' || !_strcmp(x, "&&"))
#define IS_REDIR_TOKEN(x) (x[0] == '>' || x[0] == '<')

#define true 1
#define false 0
#define PS2 "> "

typedef int (*exec_f)(char **);

/**
 * struct command_s - command struct
 * @logic:      macro that determines logic (see macros above)
 * @command:    command as a string
 * @path:       command path as a dynamically-allocated string
 * @args:       array of arguments for command (first argument must be command)
 * @input:      input name as string
 * @output:     output name as string
 * @input_fd:	input fd
 * @output_fd:	output_fd
 * @executor:	function that executes command
 **/
typedef struct command_s
{
	int logic;
	char *command;
	char *path;
	char **args;
	char *input;
	char *output;
	int input_fd;
	int output_fd;
	exec_f executor;
} command_t;

/**
 * struct alias_s - struct that defines an alias
 * @alias: alias name
 * @value: value of alias
 * @next: next alias value
 */
typedef struct alias_s
{
	char *alias;
	char *value;
	struct alias_s *next;
} alias_t;

/**
 * struct shell_s - shell struct
 * @name: shell name
 * @prompt: shell prompt
 * @history: shell history
 * @run: if shell is running
 * @lines: lines read
 * @status: shell status
 * @history_fd: history file descriptor
 * @interactive: if shell is in interactive mode
 * @history_size: size of history
 * @aliases: alias list
 **/
typedef struct shell_s
{
	char *name;
	char *prompt;
	char **history;
	int run;
	int lines;
	int status;
	int history_fd;
	int interactive;
	int history_size;
	alias_t *aliases;
} shell_t;

/* global variable declarations */
extern char **environ;
extern shell_t shell;

/* main shell function declarations */
void shell_init(char *shellname, int input);
void execute_hshrc(void);
void execute_file(int fd);
void shell_cleanup(void);
void sigint_handler(int signum);

void execute_line(char **tokens);
int fork_and_exec(command_t *cmd);
int get_IO(command_t *cmd, int prev_logic);
int get_input_fd(command_t *cmd);
int get_output_fd(command_t *cmd);
int clean_pipes(command_t *cmd);

/* Command configuration function declarations */
command_t *make_command(char **tokens, int *i);
command_t *command_config(char *path);
exec_f get_executor(char *command);
char *get_program_path(char *program);

/* built-in function declarations */
int builtin_cd(char **args);
int builtin_setenv(char **args);
int builtin_alias(char **args);
int builtin_help(char **args);
int builtin_env(char **args);
int builtin_setenv(char **args);
int builtin_unsetenv(char **args);
int builtin_history(char **args);
int builtin_exit(char **args);

/* history helper function declarations */
int get_history(char *history[]);
void save_line_to_history(char *line);
void save_history_to_file(void);

/* alias helper function declarations */
char *get_alias(char *alias);
int print_alias(char *alias);
int print_aliases(void);

/* tokenization and expansion function declarations */
char **get_tokens(int fd);
char *parse_line(char **string);
char *fix_dquote(char **line, char *token, int fd);
char *get_heredoc(char **line, int fd);
char *replace_vars(char *token);

/* error handler function declarations */
int handle_error(int code, char *program);
int handle_syntax_error(char *token);

/* prompt handling function declarations */
char *get_prompt(int fd);
char *get_date_prompt(void);
char *get_hostname_prompt(void);
char *get_username_prompt(void);
char *get_shellname_prompt(void);
char *get_cwd_prompt(void);

/* help function declarations */
int builtin_help(char **args);
void help_help(void);
void help_alias(void);
void help_exit(void);
void help_cd(void);
void help_history(void);
void help_env(void);
void help_setenv(void);
void help_unsetenv(void);

/* other helpers function declarations */
char *_getenv(char *key);
char *_realloc(char *p, int size);

#endif /* SHELL_H */
