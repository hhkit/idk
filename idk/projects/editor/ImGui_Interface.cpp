#include "pch.h"
#include "ImGui_Interface.h"

void idk::edt::I_Interface::PushImGuiFunction(std::function<void()> func)
{
	imgui_plainOldFuncList.emplace_back(std::move(func));
}

void idk::edt::I_Interface::PushImGuiFunction(void hi())
{
	std::function<void()> func = hi;
	imgui_plainOldFuncList.emplace_back(std::move(func));
}
