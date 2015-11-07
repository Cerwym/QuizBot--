#include <string>
#include <map>
#include "defines.h"

using namespace std;
class BotModule
{

public:

	// Might want to introduce a variable here that defines how many messages this module can send per X

	// IMPORTANT we want to introduce a list of available commands to listen to and the appropriate function to call when that command has been parsed.
	// FORMAT : <string> command, *function
	// When adding in the commands, we should check to see if ANY of these command codes have been registered, if they have DO NOT add the command to the list and warn the user.
	// When interfacing with the (planned) web interface, this should also be output to the user.


	BotModule();
	~BotModule(){}
	bool Init(std::map<const std::string, bool (BotModule::*)(CommandData)>* commandList,CommandData commandData);
	virtual void Shutdown(){};
	virtual void Update(){};
	bool Start(CommandData commandData);

	virtual bool MustRunEveryFrame() final { return mMustRunEveryFrame; }
	bool IsInitialized(){ return mHasInitializedSuccessfully; }
	
	std::string GetModuleName(){ return mModuleName; }

protected:

	virtual void Module_Register_Commands(){};
	virtual bool Module_Init(CommandData commandData){ return true; };
	virtual bool Module_Start(CommandData commandData){ return true; };
	virtual bool Module_Pause(CommandData commandData){ return true; };
	virtual bool Module_Resume(CommandData commandData){ return true; };
	virtual bool Module_Stop(CommandData commandData){ return true; };
	
	void RegisterModuleCommand(const std::string& commandFunctionString, bool (BotModule:: *moduleFunction)(CommandData));
	bool mHasInitializedSuccessfully = false;
	bool mHasShutDown = false;
	bool mMustRunEveryFrame = false;

	std::string mModuleName ="";
	std::string mWorkingDirectory;
	std::string mGivenOptions;

private: 

	std::map<const std::string, bool (BotModule::*)(CommandData)>* mModuleCommands;
};