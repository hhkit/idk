#pragma once
#include <script/MonoEnvironment.h>

namespace idk::mono
{
	class MonoWrapperEnvironment
		: public MonoEnvironment
	{
	public:
		MonoWrapperEnvironment(string_view full_path_to_game_dll);
		~MonoWrapperEnvironment();
	private:
	};

}