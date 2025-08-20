# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/12 16:43:52 by jpiech            #+#    #+#              #
#    Updated: 2025/08/20 16:57:44 by jpiech           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CC = c++

INCLUDES_DIR = includes

CFLAGS = -Wall -Werror -Wextra -g3 -std=c++98 -I./$(INCLUDES_DIR)

SRCS_DIR = srcs

SRCS =	$(SRCS_DIR)/main.cpp \
	$(SRCS_DIR)/Webserv.cpp \
	$(SRCS_DIR)/Server.cpp \
	$(SRCS_DIR)/Client.cpp \
	$(SRCS_DIR)/Request.cpp \
	$(SRCS_DIR)/Signals.cpp

OBJ_DIR = obj

OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

RM = rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(CFLAGS) $(OBJS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re