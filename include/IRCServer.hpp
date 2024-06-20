#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <string>
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

#define RPL_TOPIC 332
#define RPL_NOTOPIC 331
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_CHANNELMODEIS 324

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

#define CLIENT_HEADER "[client] --> "
#define SERVER_HEADER "[server] --> "
#define BROADCAST_HEADER "[server][BC] --> "

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

	typedef struct {
		bool is_register;
		std::string nickname;
		std::string username;
		std::string server_addr;
		std::vector<std::string> channels;
		
	} userInfo;

	typedef struct {
		std::string topic;
		std::vector<std::string> members;
		std::vector<std::string> operators;
		std::string key;
		int userCount;
		int userLimit;
		int isTopicProtected;
		int isInviteOnly;
	} channelInfo;

	typedef void (IRCServer::*CommandHandler)(int, std::istringstream&);

    int port_;
	std::string password_;
	int serverSocket_;

    std::vector<int> clients_;
    std::vector<struct pollfd> poll_fds_;

	std::map<int, userInfo>userInfo_; //Db des users
	std::map<std::string, channelInfo>channelInfo_; //Db des channels
	std::map<std::string, CommandHandler> commandMap_; //Pointeurs sur fonction des differentes commandes
	std::string availableModes_; //Vecteur de tout les modes possible;

	std::string getCommandPrefix(int clientSocket);
	int			getClientSocket(const std::string &nickname);
	int			getClientSocketFromUsername(const std::string &username);
	std::string getServerReply(int numeric, int clientSocket);
	std::string getMemberList(const std::string channel);

	void removeMember(int clientSocket, std::string channel);

	bool isUser(const std::string & str);
	bool isOperator(std::string nickname, std::string channel);
	bool isModeValid(std::string mode);

	void updateUserMode(int clientSocket, std::string user, std::string mode);
	void updateChannelMode(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void setOperator(std::string nickname, std::string channel, int clientSocket);

	void modeSplitter(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void keyModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void operatorModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void userLimitModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option);
	void topicLockModeManager(int clientSocket, std::string channel, std::string mode);
	void inviteModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option);

	void sendChannelMemberList(int clientSocket, std::string channel);
	void sendChannelTopic(int clientSocket, std::string channel);

	void printResponse(int mode, std::string message);

    void initializeSocket();
    void acceptConnections();
    void handleClient(int clientSocket);
    void closeSocket(int socket);
    void broadcastMessage(int senderSocket, const std::string& message, const std::string& channel);
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
    void handleCapCommand(int clientSocket, std::istringstream& lineStream);
};

#endif // IRCSERVER_HPP
