#include "BotModule.h"

#include <string>
#include <vector>
#include <chrono>

using namespace std::chrono;

class QuizModule : public BotModule
{
public:

	QuizModule();
	~QuizModule();

	// Question Set to load, (is the user an admin), how many questions, how long each round should lasts
	void Update();
	void Shutdown();
	
	void ParseAnswer(const std::string& answerData);
	bool ReadQuestionFile(const std::string& questionSetName);
	bool IsGameRunning(){ return mIsGameRunning; }
	// Start, Stop, Pause, Resume, GetScoreboard, GetAvailableCommands, Run

	//Commands
	// Help, Start(Admin), Stop(Admin), Pause(Admin), Resume(Admin)

protected:

	bool Module_Init(CommandData commandData);

	bool Module_Start(CommandData commandData);
	bool Module_Pause(CommandData commandData);
	bool Module_Resume(CommandData commandData);
	bool Module_Stop(CommandData commandData);

	void Module_Register_Commands();

private:

	bool StartGame(const std::string& questionset, const int& numOfQuestions, const int& roundTime);
	bool CheckForQuestionSets(const std::string& filenameoflist);
	void SetModuleName();

	struct QuestionInfo
	{
		bool alreadyAsked = false;
		std::string Question = "";
		std::string Answer = "";
	};

	struct QuestionData
	{
		std::string QuestionSetName = "NONE";
		std::vector<QuestionInfo> Data;
		int AvailableQuestions = 0;
	};
	
	std::vector<QuestionData*> mAvailableQuestionSets;
	QuestionData* mCurrentQuestion;
	
	steady_clock::time_point mTimeAtRoundStart;
	duration<double> mElapsedRoundTime;

	// If it's not running it's been paused or stopped.
	bool mIsGameRunning;
	int mRoundTime;
	int mNumOfQuestions;

	// scoreboard, question number, questions left, question data, timer.
};