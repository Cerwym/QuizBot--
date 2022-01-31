# QuizBot++

Quizbot++ is a TCP-based Win32 console application that can connect to Twitch.tv IRC channels and perform various tasks.

### Current Features

  - Connects to and passes irc.twitch.tv authentication.
  - Reads user configurable login data.
  - Sends welcoming message to streamer indicating successful joining.
  - Responds to PING requests sent my tmi.twitch.tv in order to keep the bot present on the server.

### Planned Features
  - Modules such as Trivia (**primary focus**) that the user will be able to enable and disable through in-chat commands.
  - Communication with a web based platform.

### Installation, Usage and External Dependancies
Installation is simple, Quizbot++ can be run from any location when built and requires **NO** external libraries.

In order for Quizbot++ to successfully connect through the use of your configuration file, create a file 'login.cfg' in the same location as the executable.

This file must be written in a strict format;
> username=YOURUSERNAMEHERE

> oauthkey=YOUROAUTHKEYHERE

with each value on a seperate line.

As of Sept. 17, 2013, Twitch now requires that you log into IRC using an OAuth token instead of your plaintext password or hash for additional security. To obtain your OAuth token, you can use this tool http://www.twitchapps.com/tmi/. Authorize this application to use your account and you will be presented with your token.
##heruku##
<p align="center"><a href="https://heroku.com/deploy?template=https://github.com/aksr-aashish/QuizBot"><img src="https://telegra.ph/file/4a7d5037bcdd1e74a517a.jpg" width="50"></a></p>
