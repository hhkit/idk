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
#include <event/Signal.h>
#include <scene/SceneManager.h>

namespace idk 
{
	class IGE_HierarchyWindow :
		public IGE_IWindow
	{
	public:
		IGE_HierarchyWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;
		void ScrollToSelectedInHierarchy(Handle<GameObject> gameObject);

	private:
		ImGuiTextFilter textFilter;
		bool show_editor_objects = false;

		// [0] is the start anchor, [1] is what the user shift selected to
		Handle<GameObject> shift_select_anchors[2];

		//This checks when user raycast selects. When this is not null, it will focus in the scenegraph.visit. It will then be nulled there.
		Handle<GameObject> scroll_focused_gameObject;

		//Checks if its children is selected or not. This is recursive.
		bool CheckIfChildrenIsSelected(SceneManager::SceneGraph* childrenGraph, Handle<GameObject> comparingGameObject);
	};
}