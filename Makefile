# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/15 11:21:29 by bvan-pae          #+#    #+#              #
#    Updated: 2024/06/17 16:49:46 by bvan-pae         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CPP     := g++
CFLAGS  := -Wall -Wextra -Werror -std=c++98

NAME    := ircserv

SRC     := $(wildcard src/*.cpp) $(wildcard src/cmds/*.cpp) main.cpp
OBJ_DIR := objects
OBJ     := $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)
OBJ_SUBDIRS := $(sort $(dir $(OBJ)))

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_SUBDIRS)
	$(CPP) $(CFLAGS) -c $< -o $@

$(OBJ_SUBDIRS):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
