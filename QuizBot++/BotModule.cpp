#include "BotModule.h"

BotModule::BotModule()
{
	// ToDo : Change this behaviour to work for additional platforms.
	mWorkingDirectory = "ModuleData\\";
	mModuleName = "NONAME";
}


// Start a module with the given command string
// We can change this to check to see if any data exists after the command AND check to see if the data matches the bots registered commands
// This will return TRUE or FALSE depending on the modules startup logic.
bool BotModule::Start(const std::string& withOptionsString)
{
	mGivenOptions = withOptionsString;
	return Module_Start();
}