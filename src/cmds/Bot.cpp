/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 14:44:40 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 16:40:51 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include "../../include/Bot.hpp"


Bot::Bot() {
    srand(time(0));
    startNewGame();
}

void Bot::startNewGame() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board_[i][j] = ' ';
        }
    }
    currentPlayer_ = 'X';
    gameOver_ = false;
    winner_ = ' ';
}

std::string Bot::makeMove(int row, int col, char player) {
    if (gameOver_ || board_[row][col] != ' ') {
        return "Invalid move!";
    }

    board_[row][col] = player;
    if (checkWin(player)) {
        gameOver_ = true;
        winner_ = player;
        return "Player " + std::string(1, player) + " wins!";
    }

    if (checkDraw()) {
        gameOver_ = true;
        return "It's a draw!";
    }

    currentPlayer_ = (currentPlayer_ == 'X') ? 'O' : 'X';
    return "Move accepted!";
}

std::string Bot::makeRandomMove() {
    if (gameOver_) {
        return "Game over!";
    }

    std::vector<std::pair<int, int> > availableMoves;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board_[i][j] == ' ') {
                availableMoves.push_back(std::make_pair(i, j));
            }
        }
    }

    if (availableMoves.empty()) {
        return "No available moves!";
    }

    int randomIndex = rand() % availableMoves.size();
    int row = availableMoves[randomIndex].first;
    int col = availableMoves[randomIndex].second;

    return makeMove(row, col, 'O');
}

std::string Bot::getBoard() const {
    std::string board;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board += (board_[i][j] == ' ') ? "_" : std::string(1, board_[i][j]);
            if (j < 2) board += "|";
        }
        board += "\n";
        if (i < 2) board += "-----\n";
    }
    return board;
}

bool Bot::isGameOver() const {
    return gameOver_;
}

std::string Bot::getWinner() const {
    if (winner_ != ' ') {
        return "Player " + std::string(1, winner_) + " wins!";
    }
    return "No winner";
}

bool Bot::checkWin(char player) const {
    for (int i = 0; i < 3; ++i) {
        if (board_[i][0] == player && board_[i][1] == player && board_[i][2] == player) return true;
        if (board_[0][i] == player && board_[1][i] == player && board_[2][i] == player) return true;
    }
    if (board_[0][0] == player && board_[1][1] == player && board_[2][2] == player) return true;
    if (board_[0][2] == player && board_[1][1] == player && board_[2][0] == player) return true;
    return false;
}

bool Bot::checkDraw() const {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board_[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

char Bot::getCurrentPlayer() const {
    return currentPlayer_;
}

void IRCServer::handleGameCommand(int clientSocket, const std::string& message) {
    std::istringstream msgStream(message);
    std::string command;
    msgStream >> command;

    if (!command.empty() && command[0] == ':') {
        command = command.substr(1);
    }

    std::cout << "Trimmed command: " << command << std::endl;
    
    if (command == "start") {
        Bot_.startNewGame();
        std::ostringstream convert;
        convert << clientSocket;
        std::string clientSocketStr = convert.str();
        std::string startMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + clientSocketStr + " :New Tic Tac Toe game started!\n" + Bot_.getBoard() + "\r\n";
        send(clientSocket, startMessage.c_str(), startMessage.length(), 0);
    } else if (command == "move") {
        int row, col;
        msgStream >> row >> col;
        
        if (msgStream.fail() || row < 0 || row > 2 || col < 0 || col > 2) {
            std::ostringstream convert;
            convert << clientSocket;
            std::string clientSocketStr = convert.str();
            std::string errorMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + clientSocketStr + " :Invalid move! Please use the format 'move <row> <col>' where row and col are between 0 and 2.\r\n";
            send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
            return;
        }

        std::string result = Bot_.makeMove(row, col, 'X');
        std::ostringstream convert;
        convert << clientSocket;
        std::string clientSocketStr = convert.str();
        std::string boardMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + clientSocketStr + " :" + result + "\n" + Bot_.getBoard() + "\r\n";
        send(clientSocket, boardMessage.c_str(), boardMessage.length(), 0);

        if (Bot_.isGameOver()) {
            std::ostringstream convert;
            convert << clientSocket;
            std::string clientSocketStr = convert.str();
            std::string gameOverMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + clientSocketStr + " :Game over! " + Bot_.getWinner() + "\r\n";
            send(clientSocket, gameOverMessage.c_str(), gameOverMessage.length(), 0);
            return;
        }

        result = Bot_.makeRandomMove();
        boardMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + clientSocketStr + " :" + result + "\n" + Bot_.getBoard() + "\r\n";
        send(clientSocket, boardMessage.c_str(), boardMessage.length(), 0);

        if (Bot_.isGameOver()) {
            std::ostringstream convert;
            convert << clientSocket;
            std::string clientSocketStr = convert.str();
            std::string gameOverMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + clientSocketStr + " :Game over! " + Bot_.getWinner() + "\r\n";
            send(clientSocket, gameOverMessage.c_str(), gameOverMessage.length(), 0);
        }
    }
}