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

#define RPL_TOPIC 332
#define RPL_NOTOPIC 331
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define CLIENT "[client] --> "
#define SERVER "[server] --> "

class IRCServer {
public:
    IRCServer(int port, const std::string& password);
    ~IRCServer();
    void start();

private:

	static IRCServer* instance_;
	std::string creationDate;

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

	std::string getCommandPrefix(int clientSocket);
	int getClientSocket(const std::string &nickname);
	std::string getServerReply(int numeric, int clientSocket);
	std::string getMemberList(const std::string channel);
	
	struct sigaction sa;
	static void cleanup();
	
    void initializeSocket();
	void initializeSignals();
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
    void handleMotdCommand(int clientSocket, std::istringstream& lineStream);

	static void handleSignal(int signal_num);
};

#endif // IRCSERVER_HPP
