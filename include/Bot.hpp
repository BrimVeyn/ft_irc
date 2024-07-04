/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 14:44:46 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 16:25:09 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
#define BOT_HPP

#include <vector>
#include <string>

class Bot {
public:
    Bot();
    void startNewGame();
    std::string makeMove(int row, int col, char player);
    std::string makeRandomMove();
    std::string getBoard() const;
    bool isGameOver() const;
    std::string getWinner() const;
    char getCurrentPlayer() const;

private:
    bool checkWin(char player) const;
    bool checkDraw() const;
    char board_[3][3];
    char currentPlayer_;
    bool gameOver_;
    char winner_;
};

#endif
