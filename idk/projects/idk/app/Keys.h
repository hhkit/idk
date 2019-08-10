#pragma once
namespace idk
{
	using CharKey = decltype('A');
	// keys go here
	enum class SysKey : unsigned char
	{
		LButton = 0,
		RButton = 1,
		MButton = 2,
		LCtrl,
		LAlt,
		LShift,
		RCtrl,
		RAlt,
		RShift,
	};
}