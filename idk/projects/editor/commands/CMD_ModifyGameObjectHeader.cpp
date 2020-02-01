#include "pch.h"
#include "CMD_ModifyGameObjectHeader.h"

namespace idk
{

    CMD_ModifyGameObjectHeader::CMD_ModifyGameObjectHeader(Handle<GameObject> game_object, 
                                                           opt<string> name, 
                                                           opt<string> tag, 
                                                           opt<LayerManager::layer_t> layer, 
                                                           opt<bool> active)
        : old_name{ game_object->Name() }, new_name{ name ? *name : old_name }
        , old_tag{ game_object->Tag() }, new_tag{ tag ? *tag : old_tag }
        , old_layer{ game_object->Layer() }, new_layer{ layer ? *layer : old_layer }
        , old_active{ game_object->ActiveSelf() }, new_active{ active ? *active : old_active }
    {
        game_object_handle = game_object;
    }

    bool CMD_ModifyGameObjectHeader::execute()
    {
        if (!game_object_handle)
            return false;
        game_object_handle->Name(new_name);
        game_object_handle->Tag(new_tag);
        game_object_handle->Layer(new_layer);
        game_object_handle->SetActive(new_active);
        return true;
    }

    bool CMD_ModifyGameObjectHeader::undo()
    {
        if (!game_object_handle)
            return false;
        game_object_handle->Name(old_name);
        game_object_handle->Tag(old_tag);
        game_object_handle->Layer(old_layer);
        game_object_handle->SetActive(old_active);
        return true;
    }

}