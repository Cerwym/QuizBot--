#define WIN32_LEAN_AND_MEAN

#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#include "TwitchBot.h"

int main()
{
	TwitchBot mBot("login.cfg");
	mBot.Connect("irc.twitch.tv", "6667", "#cerwym");
	mBot.Run();
	mBot.Shutdown();
	return 0;
}