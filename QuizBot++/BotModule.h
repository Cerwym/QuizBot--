#include <string>

class BotModule
{
public:

	BotModule();
	~BotModule(){}
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;

	bool IsInitialized(){ return mHasInitializedSuccessfully; }

protected:
	
	bool mHasInitializedSuccessfully = false;
	bool mHasShutDown = false;

	std::string mModuleName;
	std::string mWorkingDirectory;

private: 
	
};