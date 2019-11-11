#pragma once
#include <editor/commands/CMD_AddComponent.h>

namespace idk
{
	class CMD_AddBehavior 
		: public CMD_AddComponent
	{ //serialize/deserialize use text.h
	public:
		CMD_AddBehavior(Handle<GameObject> gameObject, string behavior);
		CMD_AddBehavior(Handle<GameObject> gameObject, const mono::Behavior& behavior);

		virtual bool execute() override;
		virtual bool undo() override;
	private:
		const string behavior_name;
		string saved_string;
	};
}