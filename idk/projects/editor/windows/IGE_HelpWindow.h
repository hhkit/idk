//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_HelpWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		25 NOV 2019
//@brief	

/*
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>

namespace idk {
	class IGE_HelpWindow :
		public IGE_IWindow
	{
	public:
		IGE_HelpWindow();

		virtual void Update() override;


	protected:
	private:

		const vector < std::pair<const char*, const char*>> paired_controls_1 = 
		{ 
			{"LMB:"			,"Select Objects"},
			{"RMB:"			,"Control Camera Mode"},
			{"MMD:"			,"Zoom In/Out"},
			{"ALT + RMB:"	,"Zoom In/Out"},
			{"ALT + LMB:"	,"Orbit Camera"},
			{"DEL:"			,"Delete selected gameobject(s)"},
			{"CTRL + LMB:"	,"Add/Remove to selection"},
			{"SHIFT + LMB:"	,"(On Hierarchy) Multi select objects"}
		
		};

		const vector < std::pair<const char*, const char*>> paired_controls_2 =
		{
			{"Control Camera Mode:"			,"WASD - Moves Camera, Q/E - Increase/Decrease pitch"}

		};

	};





}