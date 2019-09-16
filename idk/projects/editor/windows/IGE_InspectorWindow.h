//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		16 SEPT 2019
//@brief	

/*
This window displays the editor window where you can select and modify gameobjects.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>
#include <IncludeComponents.h>

namespace idk {

	class IGE_InspectorWindow :
		public IGE_IWindow
	{
	public:
		IGE_InspectorWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;

	
	protected:


	private:

		void DisplayNameComponent(Handle<Name>& c_name);
		void DisplayTransformComponent(Handle<Transform>& c_transform);



	};





}