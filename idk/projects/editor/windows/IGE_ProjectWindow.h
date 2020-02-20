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
#include <file/PathHandle.h>
#include <event/Signal.h>
#include <res/GenericResourceHandle.h>
#include <event/Signal.h>

namespace idk
{
	class IGE_ProjectWindow : public IGE_IWindow
	{
	public:
		IGE_ProjectWindow();

        virtual void Initialize() override;
		virtual void BeginWindow() override;
        virtual void Update() override;

        Signal<span<GenericResourceHandle>> OnAssetsSelected;
        Signal<GenericResourceHandle> OnAssetDoubleClicked;

	private:
        PathHandle current_dir;
        PathHandle clicked_path;
        PathHandle selected_path;
        bool renaming_selected_asset = false;
        bool just_rename = false;
        float icon_size = 64.0f;
        ImGuiTextFilter filter;

        void displayDir(PathHandle dir);
        void drawThumbnail(PathHandle path);
        void drawContents(vector<PathHandle>& paths, ImVec2 spacing, int icons_per_row);
        void folderDragDropTarget(PathHandle path);

        GenericResourceHandle getOrLoadFirstAsset(PathHandle path);
        string unique_new_mount_path(string_view name, string_view ext);
        string unique_new_mount_path(string_view name, string_view ext, PathHandle dir);
	};
}