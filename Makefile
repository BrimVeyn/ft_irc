# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/15 11:21:29 by bvan-pae          #+#    #+#              #
#    Updated: 2024/05/10 14:09:22 by bvan-pae         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CPP 	:= c++
CFLAGS 	:= -Wall -Wextra -Werror -std=c++98

NAME 	:= merge

SRC	:= main.cpp

OBJ 	:= $(SRC:%.cpp=%.o)

all : $(NAME)

$(NAME) : $(OBJ)
	$(CPP) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean:
	rm -rf $(OBJ) $(NAME)

re:	fclean all
