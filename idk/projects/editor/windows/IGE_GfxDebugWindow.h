#pragma once
//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_GfxDebugWindow.h
//@author	Tan Heng Chee
//@param	Email : t.hengchee\@digipen.com
//@date		30 SEPT 2019
//@brief	

/*
This window displays the editor window where you can view shadow maps.
*/
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <editor/windows/IGE_IWindow.h>
#include <imgui/imgui.h>
#include <memory>
#include <util/ExtraVars.h>
namespace idk {
	class IGE_GfxDebugWindow :
		public IGE_IWindow
	{
	public:
		IGE_GfxDebugWindow();

		virtual void Update() override;
		void RenderExtraVars(ExtraVars&);
		~IGE_GfxDebugWindow();

	protected:


	private:
		int _selected_light = 0;
		struct Pimpl;
		std::unique_ptr<Pimpl> _pimpl;
	};





}