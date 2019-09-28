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

namespace idk
{
	class IGE_ProjectWindow : public IGE_IWindow
	{
	public:
		IGE_ProjectWindow();

		virtual void BeginWindow() override;
        virtual void Update() override;

        Signal<GenericResourceHandle> OnAssetSelected;
        Signal<GenericResourceHandle> OnAssetDoubleClicked;

	private:
        PathHandle current_dir;
        PathHandle clicked_path;
        PathHandle selected_path;
        GenericResourceHandle selected_asset;

        void displayDir(PathHandle dir);
        GenericResourceHandle getAsset(PathHandle path);
	};
}