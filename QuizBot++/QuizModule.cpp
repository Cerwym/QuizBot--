#include "QuizModule.h"
#include <stdio.h>
#include <fstream>
#include <sstream>

QuizModule::QuizModule()
{
	mModuleName = "Quiz Module";

	mIsGameRunning = false;
}


QuizModule::~QuizModule()
{
	if (!mHasShutDown)
		Shutdown();
}

void QuizModule::SetModuleName()
{
	mModuleName = "QuizBot";
}

void QuizModule::Module_Register_Commands()
{
	RegisterModuleCommand("!quiz start", static_cast<bool (BotModule::*)(CommandData)> (&QuizModule::Module_Start));
	RegisterModuleCommand("!quiz pause", static_cast<bool (BotModule::*)(CommandData)> (&QuizModule::Module_Pause));
	RegisterModuleCommand("!quiz resume", static_cast<bool (BotModule::*)(CommandData)> (&QuizModule::Module_Resume));
	RegisterModuleCommand("!quiz stop", static_cast<bool (BotModule::*)(CommandData)> (&QuizModule::Module_Stop));
}

// TODO : Return available commands that we should listen to
bool QuizModule::Module_Init(CommandData commandData)
{
	// Open up file ModuleData\Questions\question_sets.txt
	// parse each line as an available question set.
	// scan in
	mCurrentQuestion = 0;
	mWorkingDirectory += "Questions\\";
	mHasInitializedSuccessfully = CheckForQuestionSets("question_sets.txt");

	mNumOfQuestions = 10;
	mRoundTime = 60;
	
	printf("Quizbot Activated\n");
	
	return mHasInitializedSuccessfully;
}

bool QuizModule::Module_Start(CommandData commandData)
{
	// TODO : this can be improved so that it comes redundant.
	if (!mHasInitializedSuccessfully)
		Module_Init(CommandData()); // Pass in empty data.


	// Split data from @param commandData and pass it to StartGame()

	// Split mGivenOptionsString here or somewhere else? here for now.
	std::string heyGivenString = mGivenOptions;

	// TEST value parameters
	return StartGame("fallout.txt", mNumOfQuestions, mRoundTime);
}

bool QuizModule::Module_Stop(CommandData commandData)
{
	printf("QuizModule::Module_Stop() STUB\n");
	return true;
}

void QuizModule::Shutdown()
{
	mHasShutDown = true;
	printf("Shutting down Quizbot...\n");
}

bool QuizModule::StartGame(const std::string& questionset, const int& numOfQuestions, const int& roundTime)
{
	// split 
	bool success = false;

	if (!mHasInitializedSuccessfully)
		return false;

	if (mIsGameRunning == false)
	{
		printf("Starting quiz...\n");
		success = ReadQuestionFile(questionset);
		mRoundTime = roundTime;
		mTimeAtRoundStart = steady_clock::now();
		mIsGameRunning = success;
		mMustRunEveryFrame = true;
	}
	else
		printf("An attempt to start a quiz was made while one was already running, ignoring...\n");

	return success;
}

bool QuizModule::Module_Pause(CommandData data)
{
	mIsGameRunning = false;
	mMustRunEveryFrame = false;

	return true;
}

bool QuizModule::Module_Resume(CommandData data)
{
	// If the param is true, reset the active round time
	mIsGameRunning = false;
	mMustRunEveryFrame = false;

	return true;
}

void QuizModule::Update()
{
	if (mIsGameRunning)
	{
		// What question are we on...
		// How long left till the end of the next round...	
		steady_clock::time_point currentlyElapsed = steady_clock::now();
		mElapsedRoundTime = duration_cast<duration<double>>(currentlyElapsed - mTimeAtRoundStart);
		if (mElapsedRoundTime.count() >= mRoundTime)
		{
			// Ask a new question.
			printf("Round time finished\n");
		}
	}
}

void QuizModule::ParseAnswer(const std::string& answerData)
{
	printf("Checking to see if '%s' was a correct answer\n", answerData.c_str());
}

bool QuizModule::ReadQuestionFile(const std::string& questionSetName)
{
	std::string toCheck = questionSetName;
	std::stringstream checkExtension;
	std::string line;
	bool extensionCheck = false;
	bool questionSetFound = false;
	QuestionData* dataToFill = 0;

	// Just perform a sanity check to see if the user has supplied a name that ends in .txt since we store that in the question set data.
	int length = questionSetName.length();

	// check to see the end of the suppled string is ".txt"
	for (int i = 4; i > 0; i--)
	{
		checkExtension << questionSetName[length - i];
	}

	if (checkExtension.str() != ".txt")
		toCheck.append(".txt");


	// We'll then need to check to see if any of the question sets contains this name.
	for (size_t i = 0; i < mAvailableQuestionSets.size(); i++)
	{
		if (mAvailableQuestionSets.at(i)->QuestionSetName == toCheck)
		{
			dataToFill = mAvailableQuestionSets.at(i);
			break;
		}
	}

	if (dataToFill != 0)
	{
		int foundQuestions = 1;
		std::stringstream fullPath;
		fullPath << mWorkingDirectory << dataToFill->QuestionSetName;
		std::ifstream infile(fullPath.str());

		// Open the file
		if (!infile.fail())
		{
			while (getline(infile, line))
			{
				// Parse this file, open it up and check to see if it has any questions in it, otherwise there is no point in adding it.
				// Read and copy a line into the collection set.
				// TODO : check to see if this contains valid data.
				std::stringstream questionStream;
				std::stringstream answerStream;
				QuestionInfo newInfo;
				bool splitRead = false;
				for (size_t i = 0; i < line.length(); ++i)
				{
					if (line[i] == ':')
					{
						splitRead = true;
						++i;
					}

					if (splitRead == false)
						questionStream << line[i];
					else
						answerStream << line[i];
				}

				newInfo.Question = questionStream.str();
				newInfo.Answer = answerStream.str();
				dataToFill->Data.push_back(newInfo);
				foundQuestions++;
			}
		}

		dataToFill->AvailableQuestions = foundQuestions;

		return true;
	}

	printf("Opening the question file has failed\n");
	return false;
}

bool QuizModule::CheckForQuestionSets(const std::string& filenameoflist)
{
	std::stringstream fullPath;
	std::string line;
	bool someData = false;
	
	fullPath << mWorkingDirectory << filenameoflist;
	
	std::ifstream infile(fullPath.str());

	// Open the file
	if (!infile.fail())
	{
		while (getline(infile, line))
		{
			// Parse this file, open it up and check to see if it has any questions in it, otherwise there is no point in adding it.
			// Read and copy a line into the collection set.
			// TODO : check to see if this contains valid data.
			QuestionData* pData = new QuestionData;
			pData->QuestionSetName = line;
			mAvailableQuestionSets.push_back(pData);
			someData = true;
		}
	}
	else
		printf("Opening the question sets file has failed\n");
	
	return someData;
}
