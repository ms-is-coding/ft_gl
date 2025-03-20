# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/06 23:24:42 by smamalig          #+#    #+#              #
#    Updated: 2025/03/20 20:43:17 by smamalig         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = libft_gl.a
AR          = ar
ARFLAGS     = rcs
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -g3 -fsanitize=address
SRCS        = main.c
OBJS        = $(SRCS:.c=.o)
INCLUDES    = -Iinclude
HEADER      = include/libft_gl.h

# Colors
RED         = \033[0;31m
GREEN       = \033[0;32m
YELLOW      = \033[0;33m
BLUE        = \033[0;34m
MAGENTA     = \033[0;35m
CYAN        = \033[0;36m
BOLD        = \033[1m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(YELLOW)Building$(RESET) $(NAME)\n" $(NAME)
	@$(AR) $(AR_FLAGS) $(NAME) $(OBJS)

%.o: %.c $(HEADER)
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(MAGENTA)Compiling$(RESET) $<\n" $(NAME)
	@$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@

clean:
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(RED)Removing$(RESET) object files\n" $(NAME)
	@rm -f $(OBJS) $(TEST_OBJS) $(BONUS_OBJS)

fclean: clean
	@printf "$(BOLD)$(BLUE)%12s$(RESET): $(RED)Removing$(RESET) executables and libraries\n" $(NAME)
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
