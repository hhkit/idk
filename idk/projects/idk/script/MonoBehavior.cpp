#include "stdafx.h"
#include "MonoBehavior.h"

namespace idk::mono
{
	//const std::string& Behavior::RescueMonoObject()
	//{
	//	// TODO: insert return statement here
	//}

	Behavior::Behavior()
	{
	}

	Behavior::Behavior(Behavior&&)
	{
	}

	Behavior& Behavior::operator=(Behavior&&)
	{
		return *this;;
	}

	Behavior::~Behavior()
	{
	}

	void Behavior::UpdateCoroutines()
	{
	}

}