#pragma once
#include <idk.h>
#include <res/Resource.h>
namespace idk
{
	class ShaderSnippet : Resource<ShaderSnippet>
	{
	public:
		string snippet;
	};
}