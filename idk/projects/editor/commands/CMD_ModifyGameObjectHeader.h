#pragma once
#include <editor/commands/ICommand.h>
#include <core/GameObject.h>
#include <common/LayerManager.h>
#include <common/TagManager.h>
#include <prefab/PrefabInstance.h>

namespace idk 
{

	// because some components are auto-added, like name, tag, layer, their handles can't be preserved
	class CMD_ModifyGameObjectHeader : public ICommand 
	{
	public:
		CMD_ModifyGameObjectHeader(Handle<GameObject> game_object,
								   opt<string> name = std::nullopt,
								   opt<string> tag = std::nullopt,
								   opt<LayerManager::layer_t> layer = std::nullopt,
								   opt<bool> active = std::nullopt);

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		const string old_name, new_name;
		const string old_tag, new_tag;
		const LayerManager::layer_t old_layer, new_layer;
		const bool old_active, new_active;
		vector<PropertyOverride> overrides_old;
	};

}