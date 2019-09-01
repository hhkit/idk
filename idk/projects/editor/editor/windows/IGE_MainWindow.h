//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>

namespace idk {
	class IGE_MainWindow :
		public IGE_IWindow
	{
	public:
		IGE_MainWindow();
		~IGE_MainWindow();


		virtual void Initialize() override;
		virtual void Update() override;
	protected:
		virtual void BeginWindow() override;
		virtual void EndWindow_V() override;
	private:


	};





}