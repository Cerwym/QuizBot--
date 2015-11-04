#include "TwitchBot.h"
#include <sstream>
#include <fstream>
#include <iosfwd>

using namespace BotCore;

void ReadDataOnSocket(void* ownerPtr, SOCKET* serverSocket);

TwitchBot::TwitchBot(char* USERNAME, char* OAUTHTOKEN)
{
	mQuizModule = 0;
	mUsername = USERNAME;
	mPassword = OAUTHTOKEN;
	mIsInitialized = true;
	printf("Bot Initialized\n");
}

TwitchBot::TwitchBot(char* USERNAME, char* OAUTHTOKEN, int flags)
{
	mQuizModule = 0;
	mUsername = USERNAME;
	mPassword = OAUTHTOKEN;
	EvaluateConstructorFlags(flags);
	mIsInitialized = true;
}

// Initialize the object by passing in a configuration file, containing relevant data
TwitchBot::TwitchBot(char* configfile)
{
	mQuizModule = 0;
	if (ReadLoginFile(configfile))
	{
		mIsInitialized = true;
	}
	else
	{	
		printf("ERROR : Invalid Configuration Data\n");
		mIsInitialized = false;
	}
}

// Initialize the object by passing in a configuration file, containing relevant data AND modules wanted at runtime
TwitchBot::TwitchBot(char* configfile, int flags)
{
	mQuizModule = 0;
	// Load the modules first.
	EvaluateConstructorFlags(flags);

	if (ReadLoginFile(configfile))
	{
		mIsInitialized = true;
	}
	else
	{
		printf("ERROR : Invalid Configuration Data\n");
		mIsInitialized = false;
	}
}

TwitchBot::~TwitchBot()
{
	// Free the associated memory we assigned with calloc() calls
	if (mUsername!= NULL)
		free(mUsername);

	if (mPassword!= NULL)
		free(mPassword);
}

void TwitchBot::InitMessageQueue()
{
	// Initialize the 'queue' system.
	mNetworkMessageQueue = new QueueT[MAX_QUEUE_SIZE];

	mNextQueueLocation = 0;
	mNextMessageToProcess = 0;

	for (int i = 0; i < MAX_QUEUE_SIZE; i++)
		mNetworkMessageQueue[i].active = false;
}

void TwitchBot::EvaluateConstructorFlags(int flags)
{
	if (flags & ConstructorFlags::QuizBot)
	{
		// This needs to be changed to create a new singleton rather than instantiate an object.
		mQuizModule = new QuizModule();
		mQuizModule->Init();
		mActiveBotModules.push_back(mQuizModule);
	}

	if (flags & ConstructorFlags::CollectionBot)
	{
		printf("No object created but sod it, this is a test step\n");
	}

	if (flags & ConstructorFlags::StubBot)
	{
		printf("My name is stubby because I'm a stub\n");
	}
}

bool TwitchBot::ReadLoginFile(char* configfile)
{
	ifstream infile(configfile);
	string line;
	char buff[100];
	int parsedline = 1;

	// Open the file
	if (!infile.fail())
	{
		while (getline(infile, line))
		{
			// Read and copy a line into a buffer.
			strncpy(buff, line.c_str(), sizeof(buff));
			buff[sizeof(buff) - 1] = 0;

			for (size_t i = 0; i < line.length(); i++)
			{
				// if the character that preceding the element in the buffer that we are reading was a '=' the data that follows this is the value we need to copy.
				if (buff[i - 1] == '=')
				{
					const size_t length = line.length();

					// When we read a line, allocate an area of memory and copy the data from the character element we are at in the buffer to that memory location.
					if (parsedline == 1)
					{
						size_t allocSize = length - i;
						mUsername = (char*)calloc(sizeof(char*), allocSize);
						strncpy(mUsername, &buff[i], length - i);
						break;
					}

					else if (parsedline == 2)
					{
						size_t allocSize = length - i;
						mPassword = (char*)calloc(sizeof(char*), allocSize);
						strncpy(mPassword, &buff[i], length - i);
						break;
					}
				}

			}
			parsedline++;
		}

		return true;
	}

	return false;
}

bool TwitchBot::Connect(char* IP, char* PORT, char* channel)
{	
	// There is no point in proceeding if the bot failed initialization
	if (!mIsInitialized)
		return false;
	else
		printf("TwitchBot Initialized\n");

	char recvbuf[4096];
	int iResult;
	int recvbuflen = 4096;
	mChannel = channel;

	if (!InitWinSock())
	{
		printf("Failure initializing WINSOCK\n");
		return false;
	}

	// Create a socket to the server
	m_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!m_ClientSocket)
	{
		printf("Failure creating client socket\n");
		return false;
	}

	mSocketMode = 0;
	// Set the socket to block on recv() sommands
	ioctlsocket(m_ClientSocket, FIONBIO, &mSocketMode);

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port;

	iResult = getaddrinfo(IP, PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d", iResult);
		Shutdown();
		return false;
	}

	// Attempt to connect to an address
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a socket for connection to server
		m_ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_ClientSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			Shutdown();
			return false;
		}
		
		// Connect to server.
		printf("Attempting to connect to server\n");
		iResult = connect(m_ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(m_ClientSocket);
			m_ClientSocket = INVALID_SOCKET;
			printf("Continuing...\n");
			continue;
		}

		break;
	}

	// All we are testing if we have made a TCP connection to the server.
	mIsConnected = true;
	InitMessageQueue();
	printf("Connected to server!\n");

	// To connect to the Twitch servers, the protocol expects the PASS followed by name
	SendIRCData(string((string)mAvailableIRCCommands.Pass += mPassword));
	SendIRCData(string((string)mAvailableIRCCommands.Nick += mUsername));

	// Send a message to the server indicating we want to receive membership information data as it is not sent by default
	SendIRCData(string(mAvailableIRCCommands.TWITCH_RequestMemStateEvents));
	// Also send a message requesting that IRC v3 tags are added to PRIVMSG, USERSTATE, NOTICE and GLOBALUSERSTATE#
	SendIRCData(string(mAvailableIRCCommands.TWITCH_RequestTags));

	// ToDo : Expand this to check if there was a message back from the server indicated failure, right now we assume a TCP connection means FULL success
	
	iResult = recv(m_ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
	{
		for (int i = 0; i < iResult; i++)
		{
			if (recvbuf[i] != '\0')
				printf("%c", recvbuf[i]);
		}
	}

	memset(recvbuf, 0, recvbuflen);

	freeaddrinfo(result);

	if (m_ClientSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		Shutdown();
		return 1;
	}
	
	SendIRCData(string((string)mAvailableIRCCommands.Join += mChannel));
	SendChannelMessage(mChannel, "Pete's TwitchBot Activated");

	mReadingThread = std::thread(&ReadDataOnSocket, (void*)this, &m_ClientSocket);
	return true;
}

bool TwitchBot::InitWinSock()
{
	WSADATA SocketData;

	int error = WSAStartup(0x0202, &SocketData);
	if (error != 0)
	{
		// Handle WSAStartup Error
		printf("InitWinSock() Failed with error code %d\n", WSAGetLastError());
		return false;
	}

	printf("WinSock initialized successfully\n");
	return true;
}

// Send a string to the client socket
bool TwitchBot::SendIRCData(string messagedata)
{
	// It makes sense to perform a user-error check here, in order for the message to be sent correctly, the data MUST end in a \n escape sequence.
	if (messagedata[messagedata.length() - 1] != '\n')
		messagedata.append("\n");

	int bytes_sent = send(m_ClientSocket, messagedata.c_str(), messagedata.length(), NULL);
	//int bytes_sent = send(m_ClientSocket, messagedata.c_str(), messagedata.length(), NULL);

	if (bytes_sent == 0)
	{
		printf("Message failed to send\n");
		return false;
	}

	return true;
}

// Send a message to the requested channel
bool TwitchBot::SendChannelMessage(char* channel, string message)
{
	string messageToSend;
	messageToSend += ((string)mAvailableIRCCommands.Message += channel).append(" :") += message;
	return SendIRCData(messageToSend);
}

// Split up and parse the message sent from the server in a readable format. OPTIMIZATION NEEDED
void TwitchBot::ParsePRIVMSG(string& data)
{
	// Split the data here into a container format, from here
	// we can check if the user is a mod, if it's a command request or something else
	PrivMsgData msgPacket;
	int sections_read = 0;
	bool headerDataRead = false;
	bool messageBegun = false;

	for (string::iterator it = data.begin(); it != data.end(); ++it)
	{
		// *it is the character code being read
		// we want to read the data up until '=' is read, then that data is the contents we care about
		// use a case statement, the case number is the data we will fill in 
		if ( it != data.begin())
		{
			if (*((it)-1) == '=')
			{
				switch (sections_read)
				{
				case 0: // Color information
					while ((*it) != ';')
					{
						msgPacket.color += (*it);
						++it;
					}
					sections_read++;
					break;
				case 1: // Display Name
					while ((*it) != ';')
					{
						msgPacket.display_name += (*it);
						++it;
					}
					sections_read++;
					break;
				case 2: // Emotes
					while ((*it) != ';')
					{
						msgPacket.emotes += (*it);
						++it;
					}
					sections_read++;
					break;
				case 3: // Subscriber
					while ((*it) != ';')
					{
						msgPacket.isSub = atoi(&(*it));
						++it;
					}
					sections_read++;
					break;
				case 4: // Turbo
					while ((*it) != ';')
					{
						msgPacket.isTurbo = atoi(&(*it));
						++it;
					}
					sections_read++;
					break;
				case 5: // UserID
					while ((*it) != ';')
					{
						msgPacket.userID += (*it);
						++it;
					}
					sections_read++;
					break;
				case 6: // user type
					while ((*it) != ' ') // this tag does not end with ';'
					{
						msgPacket.user_type += (*it);
						++it;
					}
					sections_read++;
					break;
				}
			}

			// Read in the channel
			else if (*((it)-1) == '#')
			{
				while ((*it) != ' ')
				{
					msgPacket.channel += (*it);
					++it;
				}
				headerDataRead = true;
			}
		}
		
		// Finally, when we reach the : token, copy the message in
		if (headerDataRead == true)
		{
			if (*((it) - 1) == ':')
				messageBegun = true;
		}

		if (messageBegun == true)
		{
			// do an AND comparison check to see if any of the characters are not a control code, if the result is a pass, continue with copying message in to contents
			if (  ((*it) != '\n') && ((*it) != '\r') == true) 
			{
				msgPacket.mesageContents += (*it);
			}
		}
	}
	
	// if message begins with ! check to see if any bots have registered a command to listen to
	if (msgPacket.mesageContents[0] == '!')
	{
		ParseBotCommandMessage(msgPacket);
	}

	else if (mQuizModule != 0)
	{
		if (mQuizModule->IsGameRunning())
		{
			mQuizModule->ParseAnswer(msgPacket.mesageContents);
		}
	}
	// Check to see if quizbot is running so that we can parse the message as an answer, if it's NOT running then proccess message as a command message
}

//for now QB is the only module so we should not try and pre - optimized
void TwitchBot::ParseBotCommandMessage(PrivMsgData &data)
{

	// strip all the data here and only concern ourself with characters AFTER 
	if (data.mesageContents.find("!qb hello") != string::npos)
	{
		if (data.user_type == "mod" || data.user_type == "admin")
		{
			SendChannelMessage(mChannel, string("Hello Admin, ") += data.display_name);
		}

		else
		{
			SendChannelMessage(mChannel, string("Eugh... Hello NORMAL user, ") += data.display_name);
		}
	}

	// OH MY GOD IS THIS TEMPORARY
	else if (data.mesageContents.find("!qb shutdown") != string::npos)
	{
		if (data.user_type == "mod" || data.user_type == "admin")
		{
			stringstream ss;
			ss << "Goodbye Cruel World, " << data.display_name << " hath slain me!";
			
			SendChannelMessage(mChannel, ss.str());
			mIsInLoop = false;
		}
	}

	else if (data.mesageContents.find("!quiz start") != string::npos)
	{
		// Refactor this, and all modules into something more maintainable
		if (mQuizModule == NULL)
		{
			mQuizModule = new QuizModule;
			mActiveBotModules.push_back(mQuizModule);
		}

		// temporary 
		int roundTime = 60;
		int numQuestions = 10;
		stringstream ss;

		// change the parameter start to be whatever the name is that was passed in twitch chat
		//if (mQuizModule->Start("fallout.txt", data.user_type, numQuestions, roundTime))
		if (mQuizModule->Start(data.mesageContents))
		{
			ss << data.display_name << " has started a quiz, you will have " << roundTime << " seconds to answer a question, Good Luck!";
			
			SendChannelMessage(mChannel, ss.str());
		}

		// perhaps if this fails, we could print out the available questions sets.
	}
	else if (data.mesageContents.find("!quiz pause") != string::npos)
	{
		if (data.user_type == "mod" || data.user_type == "admin")
		{
			if (mQuizModule->IsGameRunning())
				mQuizModule->Pause();
		}
	}
	else if (data.mesageContents.find("!quiz resume") != string::npos)
	{
		if (data.user_type == "mod" || data.user_type == "admin")
		{
			if (!mQuizModule->IsGameRunning())
				mQuizModule->Resume(false);
		}
	}
}

void TwitchBot::Run()
{
	// Receive until the peer closes the connection
	do {
		
		ProcessMessageQueue();
		UpdateModules();
		Sleep(16); // sleep for 16 milliseconds so we don't destroy the CPU, this isn't an intensive application
	} while (mIsInLoop == true);
}

void TwitchBot::ToggleBlockingSocket(bool setToNonBlock)
{
	// check whether or not the socket we are listening on is blocking, if it is and we must switch to asynchronous IO, switch.
	// if no registered modules require non-blocking, toggle it back on.
	int socketError = 0;
	
	if (mSocketMode == 0 && setToNonBlock == true)
	{
		mSocketMode = 1;
		socketError = ioctlsocket(mSocketMode, FIONBIO, &mSocketMode);
		printf("DEBUG : SocketMode set to NONBLOCKING\n");
		return;
	}
	else if (mSocketMode == 1 && setToNonBlock == false)
	{
		mSocketMode = 0;
		socketError = ioctlsocket(mSocketMode, FIONBIO, &mSocketMode);
		printf("DEBUG : SocketMode set to BLOCKING\n");
		return;
	}

	if (socketError != 0)
		printf("Toggle failed with code %d\n", WSAGetLastError());
}

// Update Bot Modules
void TwitchBot::UpdateModules()
{
	for (vector<BotModule*>::iterator it = mActiveBotModules.begin(); it < mActiveBotModules.end(); ++it)
	{
		if ((*it)->MustRunEveryFrame())
		{
			(*it)->Update();
		}
	}
}

void TwitchBot::Shutdown()
{
	printf("Shutting Down...\n");
	
	if (mQuizModule != 0)
		mQuizModule->Shutdown();

	closesocket(m_ClientSocket);
	WSACleanup();

	mIsConnected = false;
	mReadingThread.join();
}

void TwitchBot::ReadNetworkMessage(string& readMessage)
{
	AddMessageToQueue(readMessage);
}

void TwitchBot::AddMessageToQueue(string& message)
{
	mNetworkMessageQueue[mNextQueueLocation].messageData = message;
	
	// Set active last so that racing conditions in processing the queue do not occur.
	mNetworkMessageQueue[mNextQueueLocation].active = true;

	mNextQueueLocation++;
	if (mNextQueueLocation == MAX_QUEUE_SIZE)
		mNextQueueLocation = 0;
}

void TwitchBot::ProcessMessageQueue()
{
	// loop through all the active unprocessed messages in the queue.
	while (mNetworkMessageQueue[mNextMessageToProcess].active == true)
	{
		// coerce
		string message = mNetworkMessageQueue[mNextMessageToProcess].messageData;

		// set the message as processed.
		mNetworkMessageQueue[mNextMessageToProcess].active = false;

		// Parse block
		std::stringstream checkPong;

		for (int i = 0; i > 4; i++)
		{
			checkPong << message[i];
		}

		if (checkPong.str() == "PING")
		{
			printf("**SERVICE : Sending PONG message**\n");
			SendIRCData("PONG tmi.twitch.tv");
		}
		// Check to see if the first character is @ as currently, we are requesting IRCV3 tags to be sent along with a message
		else if (message[0] == '@')
		{
			// Convert this data in to a string, pass in iResult as the length of the new string otherwise we'd get garbage data from the size of the buffer
			ParsePRIVMSG(message);
		}
		else
		{
			printf("Bytes received: %d\n", message.length());
			printf("Not handling this message...\n");
		}

		mNextMessageToProcess++;
		if (mNextMessageToProcess == MAX_QUEUE_SIZE)
			mNextMessageToProcess = 0;
	}
}

void ReadDataOnSocket(void* ownerPtr, SOCKET* serverSocket)
{
	TwitchBot* ClassPtr;
	char recvBuffer[4096];
	int bytesRead;

	ClassPtr = (TwitchBot*)ownerPtr;
	ClassPtr->SetThreadState(true);

	while (ClassPtr->IsConnected())
	{
		// Check to see if there is a message from the server
		bytesRead = recv(*serverSocket, recvBuffer, 4096, NULL);
		if (bytesRead > 0)
		{
			// just convert it to a string, why deal with char*?
			string data(recvBuffer, bytesRead);
			ClassPtr->ReadNetworkMessage(data);
		}
	}
}

