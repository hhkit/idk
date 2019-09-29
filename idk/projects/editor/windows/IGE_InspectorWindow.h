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

		bool isComponentMarkedForDeletion = false;
		string componentNameMarkedForDeletion{}; //Is empty by default

		void displayVal(reflect::dynamic dyn);

		void DisplayNameComponent(Handle<Name>& c_name);
		void DisplayTransformComponent(Handle<Transform>& c_transform);
		void DisplayAnimatorComponent(Handle<Animator> c_anim);

		void DisplayVec3(vec3& vec);

		//Variables for vec3/vec4
		const float heightOffset = 2;
		const float widthOffset = 80;
		const float float3Size = 0.33f;
		const float float4Size = 0.25f;
		const float itemSpacing = 50;
		const float XYZSliderWidth = 10;


	};





}