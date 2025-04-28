# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/06 23:24:42 by smamalig          #+#    #+#              #
#    Updated: 2025/04/27 21:06:33 by smamalig         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = libft_gl.a
AR          = ar
ARFLAGS     = rcs
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -O3
SRC_FILES   = init.c loop.c pixel_put.c hooks.c
SRC_DIR     = src
SRCS        = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJ_DIR     = obj
OBJS        = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
INCLUDES    = -Iinclude
HEADER      = include/libft_gl.h

LDFLAGS     = -lm

RED         = \033[31m
GREEN       = \033[32m
YELLOW      = \033[33m
BLUE        = \033[34m
MAGENTA     = \033[35m
CYAN        = \033[36m
BOLD        = \033[1m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(YELLOW)Building$(RESET) $(NAME)\n" $(NAME)
	@$(AR) $(ARFLAGS) $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER)
	@mkdir -p $(dir $@)	
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(MAGENTA)Compiling$(RESET) $<\n" $(NAME)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(RED)Removing$(RESET) object files\n" $(NAME)
	@rm -f $(OBJS) $(TEST_OBJS) $(BONUS_OBJS)

fclean: clean
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(RED)Removing$(RESET) executables and libraries\n" $(NAME)
	@rm -f $(NAME)

examples/%: examples/%.c $(NAME)
	@$(CC) $(CFLAGS) $(INCLUDES) $< -L. -lft_gl $(LDFLAGS) -o $@
	@./a.out

re: fclean all

.PHONY: all clean fclean re
