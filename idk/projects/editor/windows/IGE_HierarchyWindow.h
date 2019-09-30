//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_HierarchyWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		4 SEPT 2019
//@brief	

/*
This window displays the editor window where you can select and modify gameobjects.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>

namespace idk {
	class IGE_HierarchyWindow :
		public IGE_IWindow
	{
	public:
		IGE_HierarchyWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;

	
	protected:


	private:
		ImGuiTextFilter textFilter{};

	};





}