#pragma once
#include <editor/IEditor.h>
#include <editor/ImGui_Interface.h>

namespace idk
{
	class IDE : public IEditor
	{
	public:
		IDE();

		void Init() override;
		void Shutdown() override;
		void EditorUpdate() override;
		void EditorDraw() override;
	private:
		unique_ptr<edt::I_Interface> _interface;
		//GraphicsAPI gLibVer;
	};
}