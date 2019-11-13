#include "stdafx.h"
#include "UISystem.h"
#include <app/Application.h>
#include <ui/Canvas.h>
#include <ui/RectTransform.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <scene/SceneManager.h>

namespace idk
{

    void UISystem::Update(span<class Canvas> canvases)
    {
        for (auto& c : canvases)
            ComputeCanvasHierarchyRects(c.GetHandle());
    }

    void UISystem::ComputeCanvasHierarchyRects(Handle<Canvas> canvas)
    {
        // assuming overlay space
        ivec2 screen_size = Core::GetSystem<Application>().GetScreenSize();

        auto* tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(canvas->GetGameObject());

        canvas->GetGameObject()->GetComponent<RectTransform>()->rect = rect{ vec2{0,0}, vec2{screen_size} };
        tree->visit([canvas](Handle<GameObject> child, int)
        {
            if (child->HasComponent<Canvas>())
            {
                if (child->GetComponent<Canvas>() == canvas)
                    return true;
                else
                {
                    LOG_WARNING_TO(LogPool::ANY, "Canvas cannot contain another canvas!");
                    return false;
                }
            }
            if (const auto parent = child->Parent())
            {
                const rect& parent_rect = parent->GetComponent<RectTransform>()->rect;
                const auto rect_transform_handle = child->GetComponent<RectTransform>();
                if (!rect_transform_handle)
                    return false;
                auto& rect_transform = *rect_transform_handle;
                const auto& transform = *child->Transform();

                vec2 min = parent_rect.size * rect_transform.anchor_min + rect_transform.offset_min;
                vec2 max = parent_rect.size * rect_transform.anchor_max + rect_transform.offset_max;
                rect_transform.rect.position = rect_transform.offset_min;
                rect_transform.rect.size = max - min;
            }
        });
    }

}
