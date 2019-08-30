//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_IWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	

/*
//This is an interface parent class to create other Windows to the editor
//You can also use imgui functions instead of using the inherited functions to create your own custom style.
//This is still work in progress

//When buidling window classes, public parent this class.
//You can override virtual functions to the child class to do something before or after.
//An example is shown in IGE_DebugWindow.cpp

//You cannot have windows of the same name in other classes. You can however unique identify them by adding ## in its windowName initializer

*/
//////////////////////////////////////////////////////////////////////////////////


#pragma once

#include <editorstatic/imgui/imgui.h>
//#include <imgui_stl.h>
#include <string>

namespace idk {
	class IGE_IWindow
	{

	public:
		/*--------------------------------------------------

						PUBLIC VARIABLES

		--------------------------------------------------*/

		const char* const	window_name;
		bool				is_open				{ false };		//This is used by InGameEditor
		bool				is_mouse_hovered	{ false };		//Is mouse hovering on this window?
		bool				is_window_collapsed { false };		//Is this window collapsed or not?
		ImVec2				window_size			{  };			//This is updated by EndWindow()
		ImVec2				window_position		{  };			//This is updated by EndWindow()
		ImVec2				window_pivot		{  };
		/*--------------------------------------------------
		----------------------------------------------------

					CONSTRUCTORS/DESTRUCTORS

		----------------------------------------------------
		--------------------------------------------------*/

		IGE_IWindow(char const* windowName, bool isWindowOpenByDefault, ImVec2 size = ImVec2{ 0.0f,0.0f }, ImVec2 position = ImVec2{ 0.0f ,0.0f }, ImVec2 pivot = ImVec2{ 0.0f,0.0f });

		virtual ~IGE_IWindow();

		/*--------------------------------------------------
		----------------------------------------------------

					PUBLIC ABSTRACT FUNCTIONS

		----------------------------------------------------
		--------------------------------------------------*/

		//Repositions and rescales window to how it was initialized. Note that this ignores pivoting from initialized.
		void ResetWindowPosSize();

		void DrawWindow();	//This draws the window. It is basically a wrapper to call BeginWindow and EndWindow

	protected:
		/*--------------------------------------------------

						PROTECTED VARIABLES

		--------------------------------------------------*/
		const ImVec2		start_window_position;									//Holds initialized value
		const ImVec2		start_window_size;										//Holds initialized value

		//Change these flags before BeginWindow or in Initialize if you want a different setting. Else leave them.
		//You can also use BeginWindow, or the child constructor to override these flags
		ImGuiCond_			size_condition_flags	= ImGuiCond_FirstUseEver;							//Flags that affect the Size
		ImGuiCond_			pos_condition_flags		= ImGuiCond_FirstUseEver;							//Flags that affect the Position
		ImGuiWindowFlags	window_flags			= ImGuiWindowFlags_NoSavedSettings;					//Flags that affect the Window
		ImGuiHoveredFlags_	mouse_hover_flags		= ImGuiHoveredFlags_AllowWhenBlockedByActiveItem;	//Flags that affect how the mouse is hovered

		/*--------------------------------------------------
		----------------------------------------------------

					HELPER/WRAPPER FUNCTIONS

		----------------------------------------------------
		--------------------------------------------------*/

		//Functions you can use when you inherit this class. These are mostly wrapper functions that help when creating your window
		//For each function call, it draw the next one below.

		//Initializes the window. This is for advanced use, such as modifying a variable before update or wanting a window color
		//Most of the time, this is empty.
		//This is called after ImGui is initialized.
		virtual void Initialize()	{};
		//Updates and draws the values of the window. This is the function you have to define in your derrived classes
		virtual void Update()	   = 0;


		//Begins the window. This must pair with an EndWindow!
		//The arguments set what kind of window it is or has.
		//You can override these in your child classes to do something before the window begins drawing
		virtual void BeginWindow()	{};

		//Finishes the window. Nothing else can be drawn after this function. You can still update other variables.
		//You can override these in your child classes to do something else before the window ends drawing
		virtual void EndWindow()	{};


		//Creates a "(?)" text and creates a tool tip of argument handled.
		void DrawHelpMarker(const char* desc);

		//Creates a text. Uses the printf method
		void DrawText(const char* fmt, ...);
		void DrawText(const std::string& fmt, ...);

		//Draws text with color of your choice
		void DrawTextColor(const ImVec4& color, const char* fmt, ...);
		void DrawTextColor(float r, float g, float b, float a, const char* fmt, ...);

		//The next draw function will be on the same line.
		//      pos_x == 0      : follow right after previous item
		//      pos_x != 0      : align to specified x position (relative to window/group left)
		//      spacing_w < 0   : use default spacing if pos_x == 0, no spacing if pos_x != 0
		//      spacing_w >= 0  : enforce spacing amount
		void DrawOnSameLine(float pos_x = 0.0f, float spacing_w = -1.0f);

		//Adds a vertical spacing
		void DrawSpacing();

		//Creates a button and returns bool if it is clicked.
		//Unlike ImGui::Button, this one creates a unique identifier for each button.
		//Meaning having two or more buttons with the same name will not just call the first instanced button.
		bool DrawButton(const char* label, const ImVec2& size);

		void DrawBorderRect(const ImVec2& minPos, const ImVec2& maxPos, const ImColor& color = ImColor(255, 255, 255), const float& lineSize = 1.0f);

	private:
		unsigned int itemCounter;


		void BeginWindow_V();
		void EndWindow_V();

	};

}