#include "stdafx.h"
#include "UISystem.h"
#include <app/Application.h>
#include <ui/Canvas.h>
#include <ui/RectTransform.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <scene/SceneManager.h>
#include <gfx/MaterialInstance.h>
#include <gfx/Camera.h>
#include <gfx/RenderTarget.h>

namespace idk
{
    void UISystem::LateInit()
    {
        auto frag = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/ui.frag", false);
        auto mat = Core::GetResourceManager().LoaderCreateResource<Material>(Guid{ 0x90da4f5c, 0x0453, 0x4e77, 0xbb3fb506c067d085 });
        mat->_shader_program = frag;
        mat->Name("Default UI");
        auto inst = Core::GetResourceManager().LoaderCreateResource<MaterialInstance>(UISystem::default_material_inst);
        inst->material = mat;
        inst->Name("Default UI");
        mat->_default_instance = inst;
    }

    void UISystem::Update(span<class Canvas> canvases)
    {
        for (auto& c : canvases)
            ComputeCanvasHierarchyRects(c.GetHandle());
    }

    void UISystem::ComputeCanvasHierarchyRects(Handle<Canvas> canvas)
    {
        // assuming screen space
        ivec2 screen_size = canvas->render_target->Size();

        // 1424 * 810
        auto* tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(canvas->GetGameObject());

        canvas->GetGameObject()->GetComponent<RectTransform>()->global_rect = rect{ vec2{0,0}, vec2{screen_size} };

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
            const auto parent = child->Parent();

            const auto& parent_rect_transform = *parent->GetComponent<RectTransform>();
            const rect& parent_rect = parent_rect_transform.global_rect;
            vec2 parent_pivot = parent_rect.position + parent_rect_transform.pivot * parent_rect.size;

            const auto rect_transform_handle = child->GetComponent<RectTransform>();
            if (!rect_transform_handle)
                return false;

            auto& rect_transform = *rect_transform_handle;
            const auto& transform = *child->Transform();

            vec2 min = parent_rect.size * rect_transform.anchor_min + rect_transform.offset_min;
            vec2 max = parent_rect.size * rect_transform.anchor_max + rect_transform.offset_max;
            rect_transform.global_rect.position = rect_transform.offset_min - parent_pivot;
            rect_transform.global_rect.size = max - min;

            return true;
        });
    }

}
