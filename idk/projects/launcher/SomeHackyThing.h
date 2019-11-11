#pragma once
#include <editor/IEditor.h>

namespace idk
{
	class SomeHackyThing
		: public IEditor
	{
	public:
		virtual void Init();
		virtual void LateInit();
		virtual void Shutdown();
		virtual void EditorUpdate();
		virtual void EditorDraw();

	private:
		unsigned program_id = 0;
	};
}