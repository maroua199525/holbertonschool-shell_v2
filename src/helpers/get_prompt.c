#include "../shell.h"
#include <unistd.h>
#include <stdio.h>

/**
 * get_prompt - returns the prompt, a formatted string
 * @fd: file descriptor. determines what prompt is returned
 * Return: Prompt
 */
char *get_prompt(int fd)
{
	char *(*formatters[])(void) = {&get_date_prompt, &get_hostname_prompt,
								   &get_shellname_prompt, &get_username_prompt,
								   &get_cwd_prompt, NULL};
	int i, j;
	char *prompt, *str = NULL, *tmp = NULL;
	char specifiers[] = {'d', 'H', 's', 'u', 'w', '\0'};

	if (!isatty(fd))
		return (_strdup(""));

	prompt = _getenv("PS1");
	if (!prompt || !(*prompt))
	{
		free(prompt);
		return (_strdup("$ "));
	}

	for (i = 0; prompt[i]; i++)
		if (prompt[i] == '\\')
			for (j = 0; specifiers[j]; j++)
				if (specifiers[j] == prompt[i + 1])
				{
					str = formatters[j]();
					tmp = _realloc(tmp, _strlen(str) + _strlen(prompt) - 1);
					sprintf(tmp, "%.*s%s%s", i, prompt, str, prompt + i + 2);
					free(prompt);
					free(str);
					prompt = _strdup(tmp);
					free(tmp);
					tmp = NULL;
					i -= 1;
				}

	return (prompt);
}
