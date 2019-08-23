#pragma once
#include <idk.h>

namespace idk::win
{
	class InputManager final
	{
	public:
		void SetKeyDown    (int);
		void SetKeyUp      (int);
		void SetChar       (char);

		void SwapBuffers();
		
		bool GetKeyDown  (int);
		bool GetKey      (int);
		bool GetKeyUp    (int);
		char GetChar();
	private:
		using BufType = array<bool, 0xFF>;
		BufType _input_buffers[2]{ {}, {} };
		bool    _curr_buffer{};
		char    _last_char{};

		ivec2 _curr_mouse_pos;
		ivec2 _prev_mouse_pos;

		BufType& curr_buf();
		BufType& prev_buf();
	};
}