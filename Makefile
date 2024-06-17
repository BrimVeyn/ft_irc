# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/15 11:21:29 by bvan-pae          #+#    #+#              #
#    Updated: 2024/06/17 11:07:35 by bvan-pae         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CPP     := g++
CFLAGS  := -Wall -Wextra -Werror -std=c++98

NAME    := ircserv

SRC_DIR := src
SRC     := $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJ_DIR := objects
OBJ     := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRC)))

all : $(NAME)

$(NAME) : $(OBJ)
	$(CPP) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CPP) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CPP) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
