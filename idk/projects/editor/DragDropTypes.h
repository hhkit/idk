#pragma once

#include <idk.h>
#include <res/ResourceBundle.h>

namespace idk
{
    struct ResourceBundle;

    struct DragDrop
    {
    private:

    public:
        constexpr static auto PARAMETER = "dd_param";
        constexpr static auto RESOURCE  = "dd_resource";

        static void SetResourcePayload(const GenericResourceHandle& handle);
        static void SetResourcePayload(const ResourceBundle& bundle);
        static const vector<GenericResourceHandle>& GetResourcePayloadData();
        //template<typename Res>
        //static GenericResourceHandle GetResourcePayloadData()
        //{
        //    for (auto& h : resource_payload)
        //    {
        //        if (h.resource_id() == BaseResourceID<Res>)
        //            return h;
        //    }
        //    return RscHandle<Texture>();
        //}
    };
}