#define WIN32_LEAN_AND_MEAN

#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "TwitchBot.h"

using namespace BotCore;

int main()
{
	TwitchBot mBot("login.cfg", ConstructorFlags::CollectionBot | ConstructorFlags::QuizBot | ConstructorFlags::StubBot);
	mBot.Connect("irc.twitch.tv", "6667", "#cerwym");
	mBot.Run();
	mBot.Shutdown();
	return 0;
}