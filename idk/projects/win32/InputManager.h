#pragma once
#include <idk.h>

namespace idk::win
{
	class InputManager final
	{
	public:
		void SetKeyDown    (int);
		void SetKeyUp      (int);
		void SetMouseDown  (int);
		void SetMouseUp    (int);
		void SetChar       (char);

		void SetMouseScroll(const ivec2& scroll);
		ivec2 GetMouseScroll() const;

		void SwapBuffers();
		void FlushCurrentBuffer();
		
		bool GetKeyDown  (int);
		bool GetKey      (int);
		bool GetKeyUp    (int);
		bool GetMouseDown(int);
		bool GetMouseUp  (int);
		char GetChar();
	private:
		using BufType = array<unsigned char, 0xFF>;
		BufType _input_buffers[2]{ {}, {} };
		bool    _curr_buffer{};
		char    _last_char{};

		ivec2 _curr_mouse_pos{};
		ivec2 _prev_mouse_pos{};

		ivec2 _curr_mouse_scroll{};

		BufType& curr_buf();
		BufType& prev_buf();
	};
}