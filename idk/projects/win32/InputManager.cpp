#include "pch.h"
#include "InputManager.h"

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

	void InputManager::SetMouseDragging(bool d)
	{
		_dragging = d;
	}

	void InputManager::SwapBuffers()
	{
		prev_buf() = curr_buf();
		_curr_buffer = !_curr_buffer;
		_last_char = '\0';
	}

	bool InputManager::GetKeyDown(int index)
	{
		return prev_buf()[index] == false && curr_buf()[index] == true;
	}
	bool InputManager::GetKey(int index)
	{
		return curr_buf()[index];
	}
	bool InputManager::GetKeyUp(int index)
	{
		return curr_buf()[index] == false && prev_buf()[index] == true;
	}
	bool InputManager::GetMouseDown(int index)
	{
		return prev_buf()[index] == false && curr_buf()[index] == true;
	}
	bool InputManager::GetMouseUp(int index)
	{
		return curr_buf()[index] == false && prev_buf()[index] == true;
	}
	bool InputManager::IsMouseDragging(int index)
	{
		return curr_buf()[index] && _dragging;
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
