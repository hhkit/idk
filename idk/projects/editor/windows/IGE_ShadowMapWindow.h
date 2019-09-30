//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_ShadowMapWindow.h
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

namespace idk {
	class IGE_ShadowMapWindow:
		public IGE_IWindow
	{
	public:
		IGE_ShadowMapWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;
		~IGE_ShadowMapWindow();

	protected:


	private:
		int _selected_light=0;
		struct Pimpl;
		std::unique_ptr<Pimpl> _pimpl;
	};





}