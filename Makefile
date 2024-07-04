# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/04 17:28:17 by albeninc          #+#    #+#              #
#    Updated: 2024/07/04 17:28:26 by albeninc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CPP     := c++
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
