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

	bool Init();
	// Question Set to load, (is the user an admin), how many questions, how long each round should lasts
	bool Start(const std::string& questionset, const std::string& userType, const int& numOfQuestions, const int& roundTime);
	void Shutdown();
	void ParseAnswer(const std::string& answerData);
	bool ReadQuestionFile(const std::string& questionSetName);
	bool IsGameRunning(){ return mIsGameRunning; }

	void Update();
	void Pause();
	void Resume(bool resetRoundTime);
	// Start, Stop, Pause, Resume, GetScoreboard, GetAvailableCommands, Run

	//Commands
	// Help, Start(Admin), Stop(Admin), Pause(Admin), Resume(Admin)

private:

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
	};
	
	std::vector<QuestionData*> mAvailableQuestionSets;
	QuestionData* mCurrentQuestion;
	
	steady_clock::time_point mTimeAtRoundStart;
	duration<double> mElapsedRoundTime;

	// If it's not running it's been paused or stopped.
	bool mIsGameRunning;
	int mRoundTime = 60;
	int tempcomparison = 0;

	// scoreboard, question number, questions left, question data, timer.
};