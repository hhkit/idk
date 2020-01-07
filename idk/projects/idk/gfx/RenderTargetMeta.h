#pragma once
#include <idk.h>
#include <reflect/enum_type.inl>

namespace idk
{
	ENUM(RenderTargetFormat, char,
		RGB
		);

	ENUM(AntiAliasing, char
		, None
		, _2_Samples
		, _4_Samples
		, _8_Samples
	);
}