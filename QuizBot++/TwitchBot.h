#include <string>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

class TwitchBot
{
public : 
	
	TwitchBot(char* loginfile);
	TwitchBot(char* USERNAME, char* OAUTHTOKEN);
	~TwitchBot();

	bool Connect(char* IP, char* PORT, char* channel);
	bool IsConnected() { return mIsConnected; }
private:

	bool ReadLoginFile(char* file);
	bool InitWinSock();
	bool SendIRCData(string messagedata);
	bool SendChannelMessage(char* channel, string message);

	char* mUsername = NULL;
	char* mPassword = NULL;
	char* mServer_IP;
	unsigned short m_ServerPort;
	char* mChannel;

	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	SOCKET m_ClientSocket = INVALID_SOCKET;
	int mAddressLength;
	bool mIsInitialized = false;
	bool mIsConnected = false;

	struct IRC_COMMANDS 
	{
		const string Join = "JOIN ";
		const string Nick = "NICK ";
		const string Pass = "PASS ";
		const string Message = "PRIVMSG ";
	};

	IRC_COMMANDS mAvailableIRCCommands;
};