#include <string>

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
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Update() = 0;
	bool Start(const std::string& withOptions);
	virtual bool MustRunEveryFrame() final { return mMustRunEveryFrame; } // Disallow an inherited module to use override this function
	
	bool IsInitialized(){ return mHasInitializedSuccessfully; }
	std::string GetModuleName(){ return mModuleName; }

protected:
	
	virtual bool Module_Start() = 0;

	bool mHasInitializedSuccessfully = false;
	bool mHasShutDown = false;
	bool mMustRunEveryFrame = false;

	// I would like to introduce a function here that will read in a 'Module File' in a serialize format to reduce magic values and non-transparency in regards to module conventions
	// boolParseModuleFile()

	std::string mModuleName;
	std::string mWorkingDirectory;
	std::string mGivenOptions;

private: 
	
};