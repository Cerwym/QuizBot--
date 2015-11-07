#include "BotModule.h"

BotModule::BotModule()
{
	// ToDo : Change this behaviour to work for additional platforms.
	mWorkingDirectory = "ModuleData\\";
	mModuleName = "NONAME";
	mModuleCommands = 0;
}

bool BotModule::Init(std::map<const std::string, bool (BotModule::*)(CommandData commandData)>* commandList, CommandData commandData)
{
	mModuleCommands = commandList;

	Module_Register_Commands();
	return Module_Init(commandData);
}

// Start a module with the given command string
// We can change this to check to see if any data exists after the command AND check to see if the data matches the bots registered commands
// This will return TRUE or FALSE depending on the modules startup logic.
bool BotModule::Start(CommandData commandData)
{
	return Module_Start(commandData);
}

// Receive a pointer to a function that matches the format of returning a bool while having a CommandData struct as an argument
void BotModule::RegisterModuleCommand(const std::string& commandFunctionString, bool (BotModule:: *moduleFunction)(CommandData))
{
	mModuleCommands->insert(std::make_pair(commandFunctionString, moduleFunction));
}
