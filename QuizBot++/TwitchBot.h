#include <string>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>

// Modules
#include "QuizModule.h"

using namespace std;

namespace BotCore
{

	struct ConstructorFlags
	{
		enum Values
		{
			// Enumerate in powers of two (for obvious reasons)
			QuizBot = 0x01,
			CollectionBot = 0x02,
			StubBot = 0x04
		};
	};


	class TwitchBot
	{
	public:

		TwitchBot(char* USERNAME, char* OAUTHTOKEN);
		TwitchBot(char* USERNAME, char* OAUTHTOKEN, int flags);
		TwitchBot(char* loginfile);
		TwitchBot(char* loginfile, int flags);
		~TwitchBot();

		bool Connect(char* IP, char* PORT, char* channel);
		bool IsConnected() { return mIsConnected; }
		void Run();
		void UpdateModules();
		void Shutdown();
		void ReadNetworkMessage(string& readMessage);
		void SetThreadState(bool state){ mReadThreadActive = state; }

	private:

		struct PrivMsgData
		{
			string color = "";
			string display_name = "";
			string emotes = "";
			int isSub = 0;
			int isTurbo = 0;
			string userID = "";
			string user_type = "";
			string channel = "";
			string mesageContents = "";
		};

		struct QueueT
		{
			bool active;
			struct sockaddr_in address;
			string messageData;
		};

		void EvaluateConstructorFlags(int flags);
		bool ReadLoginFile(char* file);
		
		bool InitWinSock();
		void InitMessageQueue();
		bool SendIRCData(string messagedata);
		bool SendChannelMessage(char* channel, string message);
		void ParsePRIVMSG(string& data);
		void ParseBotCommandMessage(PrivMsgData &data);

		void ToggleBlockingSocket(bool shouldBlock);
		void AddMessageToQueue(string& message);
		void ProcessMessageQueue();

		char* mUsername = NULL;
		char* mPassword = NULL;
		char* mServer_IP;
		unsigned short m_ServerPort;
		char* mChannel;

		struct addrinfo *result = NULL,
			*ptr = NULL,
			hints;

		SOCKET m_ClientSocket = INVALID_SOCKET;
		// flag to store the I/O mode of the socket. 0 = BLOCKING, 1 = NONBLOCKING
		u_long mSocketMode;
		int mAddressLength;
		bool mIsInitialized = false;
		bool mIsConnected = false;
		bool mReadThreadActive = false;
		thread mReadingThread;
		QueueT* mNetworkMessageQueue;
		int mNextQueueLocation, mNextMessageToProcess;
		const int MAX_QUEUE_SIZE = 200;
		
		bool mSocketEnableBlocking = true;

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

		IRC_COMMANDS mAvailableIRCCommands;

		// Temporary
		bool mIsInLoop = 1;

		QuizModule* mQuizModule;
		vector<BotModule*> mActiveBotModules;
		bool mHasModuleToUpdate = false;
	};
}