# -*- MakeFile -*-

CC = gcc
SRC = src/0-main.c src/1-get_tokens.c src/2-command_config.c src/3-get_IO.c src/4-execute.c src/helpers/_getenv.c src/helpers/_getline.c src/helpers/handle_error.c src/helpers/prompt_helpers.c src/helpers/get_prompt.c src/helpers/_realloc.c src/built-ins/alias.c src/built-ins/cd.c src/built-ins/env.c src/built-ins/setenv.c src/built-ins/unsetenv.c src/built-ins/exit.c src/built-ins/history.c src/built-ins/help.c src/strings/_strcat.c src/strings/_strcmp.c src/strings/_strcpy.c src/strings/_strdup.c src/strings/_strlen.c src/strings/_puts.c src/strings/_realloc_string_array.c
OBJ = $(SRC:.c=.o)
NAME = hsh
RM = rm -f
CFLAGS = -Wall -Werror -Wextra -pedantic -g

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

fclean: oclean clean

oclean:
	$(RM) $(OBJ)

clean:
	$(RM) *~ $(NAME)

re: oclean all
