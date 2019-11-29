#include "pch.h"
#include "InputManager.h"
#include <Windows.h>
namespace idk::win
{
	void InputManager::SetKeyDown(int index)
	{
		curr_buf()[index] = true;
	}
	void InputManager::SetKeyUp(int index)
	{
		curr_buf()[index] = false;
	}

	void InputManager::SetMouseDown(int index)
	{
		curr_buf()[index] = true;
	}

	void InputManager::SetMouseUp(int index)
	{
		curr_buf()[index] = false;
	}

	void InputManager::SetChar(char c)
	{
		_last_char = c;
	}

	void InputManager::SetMouseScroll(const ivec2& scroll)
	{
		_curr_mouse_scroll = scroll;
	}

	ivec2 InputManager::GetMouseScroll() const
	{
		return _curr_mouse_scroll;
	}

	void InputManager::SwapBuffers()
	{
		prev_buf() = curr_buf();
		_curr_buffer = !_curr_buffer;
		_last_char = '\0';
		_curr_mouse_scroll = ivec2{};
	}

	void InputManager::FlushCurrentBuffer()
	{
		for (auto& elem : curr_buf())
			elem = false;
	}

	bool InputManager::GetKeyDown(int index)
	{
		return s_cast<bool>(!prev_buf()[index] && curr_buf()[index]);
	}
	bool InputManager::GetKey(int index)
	{
		return s_cast<bool>(curr_buf()[index]);
	}
	bool InputManager::GetKeyUp(int index)
	{
		return s_cast<bool>(curr_buf()[index] == 0 && prev_buf()[index]);
	}
	bool InputManager::GetMouseDown(int index)
	{
		return s_cast<bool>(prev_buf()[index] == 0 && curr_buf()[index]);
	}
	bool InputManager::GetMouseUp(int index)
	{
		return s_cast<bool>(curr_buf()[index] == 0 && prev_buf()[index]);
	}
	char InputManager::GetChar()
	{
		return _last_char;
	}
	InputManager::BufType& InputManager::curr_buf()
	{
		return _input_buffers[_curr_buffer];
	}
	InputManager::BufType& InputManager::prev_buf()
	{
		return _input_buffers[!_curr_buffer];
	}
}
