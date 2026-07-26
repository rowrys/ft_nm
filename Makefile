CC := cc
NAME := ft_nm
SRCS_DIR := srcs/
SOURCES :=	nm.c		\
			file.c		\
			arguments.c	\
			parse_elf.c	\

SOURCES	:=	$(addprefix $(SRCS_DIR), $(SOURCES))

OBJ_DIR := .build/
OBJS := $(SOURCES:$(SRCS_DIR)%.c=$(OBJ_DIR)%.o)

DEPS := $(SOURCES:$(SRCS_DIR)%.c=$(OBJ_DIR)%.d)

CFLAGS := -MP -MMD -Wall -Werror -Wextra -std=gnu11 -pedantic-errors -g
CFLAGS_TEST := -g
INCLUDE := -I includes/

ifeq ($(MAKECMDGOALS),debug)
CFLAGS += -D DEBUG
endif

all: $(NAME)
debug: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(INCLUDE) -o $(NAME)

$(OBJ_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@

gdb: $(NAME)
	gdb -tui $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)
	rm -fr $(BIN)

re: fclean all

.PHONY: all debug fclean clean re gdb

-include $(DEPS)