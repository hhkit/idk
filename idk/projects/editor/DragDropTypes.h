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
    };
}