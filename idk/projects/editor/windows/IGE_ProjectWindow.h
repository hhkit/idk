//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_ProjectWindow.h
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
	class IGE_ProjectWindow :
		public IGE_IWindow
	{
	public:
		IGE_ProjectWindow();

		virtual void BeginWindow() override;
        virtual void Update() override;

	
	protected:


	private:
        void displayDir(const string& dir);
        string selected_dir;
        string selected_asset;
	};





}