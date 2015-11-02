#include <string>


class QuizModule
{
public:

	// TO DO : make this a singleton object so that it can be created at runtime and be reffered to whenever
	QuizModule();
	~QuizModule();

	bool Start(const std::string& questionset, const std::string& userType, const int& roundTime);


	// Start, Stop, Pause, Resume, GetScoreboard, GetAvailableCommands, Run

	//Commands
	// Help, Start(Admin), Stop(Admin), Pause(Admin), Resume(Admin)

private:

	// scoreboard, question number, questions left, question data, timer.
};