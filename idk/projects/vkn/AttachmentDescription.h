#pragma once
#include <idk.h>
namespace idk::vkn
{

	struct AttachmentDescription
	{
		string_view name;
		ivec2 size;
		//Format format;
	};
}