#pragma once
#include <idk.h>
#include <core/Core.h>
#include <res/ResourceManager.h>
#include <editor/compiler/Importer.h>
#include <util/ioutils.h>

namespace idk
{
	template<typename T>
	class BypassImporter
		: public Importer
	{
	public:
		MetaBundle Import(PathHandle path, const MetaBundle& old_meta)
		{
			auto serialized_meta = old_meta.FetchMeta<T>();
			auto guid = serialized_meta ? serialized_meta->guid : Guid::Make();

			auto stream = path.Open(FS_PERMISSIONS::READ);
			auto new_val = Core::GetResourceManager().Create<T>(guid);
			parse_text(stringify(stream), *new_val);

			MetaBundle retval;
			retval.Add(new_val);
			return retval;
		}
	};
}