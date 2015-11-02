#include <string>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>

using namespace std;

class TwitchBot
{
public : 
	
	TwitchBot(char* loginfile);
	TwitchBot(char* USERNAME, char* OAUTHTOKEN);
	~TwitchBot();

	bool Connect(char* IP, char* PORT, char* channel);
	bool IsConnected() { return mIsConnected; }
	void Run();
	void Shutdown();
private:

	bool ReadLoginFile(char* file);
	bool InitWinSock();
	bool SendIRCData(string messagedata);
	bool SendChannelMessage(char* channel, string message);
	void ParsePRIVMSG(string& data);

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
	
	const enum USERFLAG
	{
		EMPTY, MOD, GLOBAL_MOD, ADMIN, STAFF
	};

	struct IRC_COMMANDS 
	{
		const string Join = "JOIN ";
		const string Nick = "NICK ";
		const string Pass = "PASS ";
		const string Message = "PRIVMSG ";
		const string TWITCH_RequestMemStateEvents = "CAP REQ :twitch.tv/membership";
		const string TWITCH_RequestTags = "CAP REQ :twitch.tv/tags";
	};

	struct PrivMsgData
	{
		string color="";
		string display_name="";
		string emotes="";
		int isSub = 0;
		int isTurbo = 0;
		string userID = "";
		string user_type = "";
		string channel = "";
		string mesageContents ="";
	};

	IRC_COMMANDS mAvailableIRCCommands;
};