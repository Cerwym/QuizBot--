#include "QuizModule.h"
#include <stdio.h>

QuizModule::QuizModule()
{
	printf("Quiz Module Initialized - I don't really do much at the moment\n");
}

QuizModule::~QuizModule()
{
	printf("Running Program... Hugs :(\n");
}

bool QuizModule::Start(const std::string& questionset, const std::string& userType, const int& roundTime)
{
	if (userType == "mod" || "admin")
	{
		printf("Admin has started a Quiz : Question set = %s\n", questionset.c_str());
		return true;
	}

	return false;
}