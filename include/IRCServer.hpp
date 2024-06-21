/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 10:04:19 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 11:56:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <string>
#include <csignal>
#include <vector>
#include <map>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

#define RESET_COLOR "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

//-------RPL response--------//
#define RPL_TOPIC 332
#define RPL_NOTOPIC 331
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_CHANNELMODEIS 324
#define RPL_INVITING 341 
//---------------------------//

//-------------ERRORS--------//
#define ERR_UNKNOWNMODE 472
#define ERR_CHANOPRIVSNEEDED 482
#define ERR_KEYSET 467
#define ERR_NEEDMOREPARAMS 461
#define	ERR_BADCHANNELKEY 475
#define ERR_CHANNELISFULL 471
#define ERR_INVITEONLYCHAN 473
#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_USERNOTINCHANNEL 441
#define ERR_PASSWDMISMATCH 464
#define ERR_ERRONEUSNICKNAME 432
//----------------------------//


#define CLIENT_HEADER "[client] --> "
#define SERVER_HEADER "[server] --> "
#define BROADCAST_HEADER "[server][BC] --> "
#define SERVER_NAME ":ft_irc "
#define DEFAULT_KICK_REASON "Kicked by an operator"

typedef struct channelInfo {
	std::string topic;
	std::vector<std::string> members;
	std::vector<std::string> operators;
	std::vector<std::string> inviteList;
	std::string key;
	int userCount;
	int userLimit;
	int isTopicProtected;
	int isInviteOnly;

} channelInfo;

typedef struct userInfo {
	bool is_authenticated;
	std::string password;
	std::string nickname;
	std::string username;
	std::string server_addr;
	std::vector<std::string> channels;

} userInfo;

enum {
	CLIENT,
	BROADCAST,
	SERVER,
};

class IRCServer {
public:
    IRCServer(int port, const std::string& password);
    ~IRCServer();
    void start();

private:

	typedef void (IRCServer::*CommandHandler)(int, std::istringstream&);

	static IRCServer* instance_;
	std::string creationDate_;
	std::string bancommands_;

    int port_;
	std::string password_;
	int serverSocket_;
	
    std::vector<int> clients_;
    std::vector<struct pollfd> poll_fds_;

	std::map<int, userInfo>userInfo_; //Db des users
	std::map<std::string, channelInfo>channelInfo_; //Db des channels
	std::map<std::string, CommandHandler> commandMap_; //Pointeurs sur fonction des differentes commandes
	std::string availableModes_; //Vecteur de tout les modes possible;
	
	void updateNickInChannels(const std::string& oldNick, const std::string& newNick);
	//-------Authentication-----//
	void authenticateClient(int clientSocket);
	void handleNickCollision(int clientSocket, std::string & nickname);
	void handleUsernameCollision(int clientSocket, std::string & username);
	void sendWelcomeMessages(int clientSocket);
	//------------------------//

	//-------Getters/Setters command------//
	std::string getCommandPrefix(int clientSocket);
	int			getClientSocket(const std::string &nickname);
	int			getClientSocketFromUsername(const std::string &username);
	std::string getServerReply(int numeric, int clientSocket);
	std::string getMemberList(const std::string channel);
	bool		isUser(const std::string & str);
	bool		isOperator(std::string nickname, std::string channel);
	bool		isModeValid(std::string mode);
	void		updateUserMode(int clientSocket, std::string user, std::string mode);
	void		updateChannelMode(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	//------------------------//

	void removeMember(int clientSocket, std::string channel); //Remove a member from a channel (KICK / PART)//

	//-------Mode command------//
	void modeSplitter(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void keyModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void operatorModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void userLimitModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void topicLockModeManager(int clientSocket, std::string channel, std::string mode);
	void inviteModeManager(int clientSocket, std::string channel, std::string mode);
	//------------------------//

	std::string gatherModes(std::string channel);

	//-------Join command------//
	void sendChannelMemberList(int clientSocket, std::string channel);
	void sendChannelTopic(int clientSocket, std::string channel);
	void sendJoinMessage(int clientSocket, std::string channel);
	//------------------------//

	void printResponse(int mode, std::string message);
	void broadcastMessage(int senderSocket, const std::string& message, const std::string& channel);

	struct sigaction sa;
	static void cleanup();
	
	//------------Socker related functions---//
    void initializeSocket();
	void initializeSignals();
    void acceptConnections();
    void handleClient(int clientSocket);
    void closeSocket(int socket);
	//--------------------------------------//

	//--------------------------Commnad related functions------------------//
	void handleCmds(std::string message, int clientSocket);
    void handleNickCommand(int clientSocket, std::istringstream& lineStream);
    void handleUserCommand(int clientSocket, std::istringstream& lineStream);
    void handleJoinCommand(int clientSocket, std::istringstream& lineStream);
	void handlePartCommand(int clientSocket, std::istringstream& lineStream);
    void handlePrivmsgCommand(int clientSocket, std::istringstream& lineStream);
    void handleKickCommand(int clientSocket, std::istringstream& lineStream);
    void handleInviteCommand(int clientSocket, std::istringstream& lineStream);
    void handleTopicCommand(int clientSocket, std::istringstream& lineStream);
    void handleModeCommand(int clientSocket, std::istringstream& lineStream);
    void handlePassCommand(int clientSocket, std::istringstream& lineStream);
	//---------------------------------------------------------------------//

	static void handleSignal(int signal_num);
};

#endif // IRCSERVER_HPP
