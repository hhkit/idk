#pragma once
#include <idk.h>
#include <res/Resource.h>
namespace idk
{
	class ShaderSnippet : public Resource<ShaderSnippet>
	{
	public:
		string snippet;
	};
}