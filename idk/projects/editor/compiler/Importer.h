#pragma once
#include <idk.h>
#include <file/PathHandle.h>
#include <res/MetaBundle.h>

namespace idk
{
	class Importer
	{
	public:
		virtual MetaBundle Import(PathHandle path, const MetaBundle& old_meta) = 0;
		virtual ~Importer() = default;
	};
}