#pragma once
#include <script/MonoEnvironment.h>

namespace idk::mono
{
	class MonoWrapperEnvironment
		: public MonoEnvironment
	{
	public:
		MonoWrapperEnvironment(string_view full_path_to_game_dll);
		void Init() override;
		bool IsPrivate(MonoClassField* field);
		bool IsAbstract(MonoType* type);
		~MonoWrapperEnvironment();
	private:
		MonoMethod* main = nullptr;
		void BindCoreFunctions();
	};

}