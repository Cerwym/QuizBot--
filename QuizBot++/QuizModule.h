#include "BotModule.h"

#include <string>
#include <vector>

class QuizModule : public BotModule
{
public:

	QuizModule();
	~QuizModule();

	bool Init();
	bool Start(const std::string& questionset, const std::string& userType, const int& roundTime);
	void Shutdown();
	void ParseAnswer(const std::string& answerData);
	bool ReadQuestionFile(const std::string& questionSetName);


	// Start, Stop, Pause, Resume, GetScoreboard, GetAvailableCommands, Run

	//Commands
	// Help, Start(Admin), Stop(Admin), Pause(Admin), Resume(Admin)

private:

	bool CheckForQuestionSets(const std::string& filenameoflist);
	void SetModuleName();
	// Question Set Name
	// Data

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

	bool mContainsQuestions;
	int mRoundTime = 60;
	QuestionData* mCurrentQuestion;
	// scoreboard, question number, questions left, question data, timer.
};