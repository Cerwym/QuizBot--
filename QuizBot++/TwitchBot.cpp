#include "TwitchBot.h"
#include <fstream>
#include <string>

TwitchBot::TwitchBot(char* USERNAME, char* OAUTHTOKEN)
{
	mUsername = USERNAME;
	mPassword = OAUTHTOKEN;

	mIsInitialized = true;
	printf("Bot Initialized\n");
}

// Initialize the object by passing in a configuration file, containing relevant data
TwitchBot::TwitchBot(char* configfile)
{
	if (ReadLoginFile(configfile))
		mIsInitialized = true;
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
	m_ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);
	if (!m_ClientSocket)
	{
		printf("Failure creating client socket\n");
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port;

	iResult = getaddrinfo(IP, PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d", iResult);
		
		// change a false return to use TwitchBot::Cleanup()
		WSACleanup();
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
			WSACleanup();
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
	printf("Connected to server!\n");

	// To connect to the Twitch servers, the protocol expects the PASS followed by name
	SendIRCData(string((string)mAvailableIRCCommands.Pass += mPassword));
	SendIRCData(string((string)mAvailableIRCCommands.Nick += mUsername));

	iResult = recv(m_ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
	{
		for (int i = 0; i < iResult; i++)
		{
			if (recvbuf[i] != '\0')
				printf("%c", recvbuf[i]);
		}
	}

	freeaddrinfo(result);

	if (m_ClientSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	
	SendIRCData(string((string)mAvailableIRCCommands.Join += mChannel));

	SendChannelMessage(mChannel, "Pete's TwitchBot Activated");
	// Receive until the peer closes the connection
	do {

		iResult = recv(m_ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			for (int i = 0; i < iResult; i++)
			{
				printf("%c",recvbuf[i]);
			}

			if (recvbuf[0] == 'P' && recvbuf[1] == 'I' && recvbuf[2] == 'N' && recvbuf[3] == 'G')
			{
				printf("**SERVICE : Sending PONG message**");
				SendIRCData("PONG tmi.twitch.tv");
			}
			printf("\n");
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(m_ClientSocket);
	WSACleanup();
	
	return true;
}

bool TwitchBot::InitWinSock()
{
	WSADATA SocketData;
	WSAPROTOCOL_INFOW* protocolBuffer;
	unsigned long bufferSize;
	int protocols[2];

	int error = WSAStartup(0x0202, &SocketData);
	if (error != 0)
	{
		// Handle WSAStartup Error
	}

	// Request the buffer size needed for holding the available protocols
	WSAEnumProtocols(NULL, NULL, &bufferSize);

	// Create a buffer for the protocol information structures
	protocolBuffer = new WSAPROTOCOL_INFOW[bufferSize];
	if (!protocolBuffer)
		return false;

	protocols[0] = IPPROTO_TCP;
	protocols[1] = IPPROTO_UDP;

	error = WSAEnumProtocols(protocols, protocolBuffer, &bufferSize);
	if (error == SOCKET_ERROR)
		return false;

	delete[] protocolBuffer;
	protocolBuffer = 0;

	printf("WinSock initialized successfully\n");
	return true;
}

// Send a string to the client socket. WARNING : a '\n' character code will be appended to this message
bool TwitchBot::SendIRCData(string messagedata)
{
	messagedata.append("\n");

	int bytes_sent = send(m_ClientSocket, messagedata.c_str(), messagedata.length(), NULL);

	if (bytes_sent == 0)
	{
		printf("Message failed to send\n");
		return false;
	}

	return true;
}

bool TwitchBot::SendChannelMessage(char* channel, string message)
{
	string messageToSend;
	messageToSend += ((string)mAvailableIRCCommands.Message += channel).append(" :") += message;
	return SendIRCData(messageToSend);
}