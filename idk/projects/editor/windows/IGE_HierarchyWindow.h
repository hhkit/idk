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

namespace idk {
	class IGE_HierarchyWindow :
		public IGE_IWindow
	{
	public:
		IGE_HierarchyWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;
		void ScrollToSelectedInHierarchy(Handle<GameObject> gameObject);
        
		Signal<> OnGameObjectSelectionChanged;

	protected:


	private:
		ImGuiTextFilter textFilter{};
		bool show_editor_objects{};

		Handle<GameObject> scroll_focused_gameObject;		//This checks when user raycast selects. When this is not null, it will focus in the scenegraph.visit. It will then be nulled there.

		bool CheckIfChildrenIsSelected(SceneManager::SceneGraph* childrenGraph, Handle<GameObject> comparingGameObject); //Checks if its children is selected or not. This is recursive.
	};





}