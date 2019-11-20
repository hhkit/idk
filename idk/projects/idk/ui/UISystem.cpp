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
#include <math/matrix_transforms.h>

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

    void UISystem::FinalizeMatrices(span<class Canvas> canvases)
    {
        for (auto& c : canvases)
            FinalizeMatrices(c.GetHandle());
    }

    void UISystem::ComputeCanvasHierarchyRects(Handle<Canvas> canvas)
    {
        // assuming screen space
        ivec2 screen_size = canvas->render_target->Size();

        // 1424 * 826
        auto* tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(canvas->GetGameObject());

        canvas->GetGameObject()->GetComponent<RectTransform>()->_local_rect = rect{ vec2{0,0}, vec2{screen_size} };

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
            const rect& parent_rect = parent_rect_transform._local_rect;
            vec2 parent_pivot = parent_rect_transform.pivot * parent_rect.size;

            const auto rect_transform_handle = child->GetComponent<RectTransform>();
            if (!rect_transform_handle)
                return false;

            auto& rect_transform = *rect_transform_handle;
            const auto& transform = *child->Transform();

            vec2 min = parent_rect.size * rect_transform.anchor_min + rect_transform.offset_min;
            vec2 max = parent_rect.size * rect_transform.anchor_max + rect_transform.offset_max;
            rect_transform._local_rect.position = min - parent_pivot;
            rect_transform._local_rect.size = max - min;

            return true;
        });
    }

    void UISystem::FinalizeMatrices(Handle<Canvas> canvas)
    {
        // assuming screen space
        ivec2 screen_size = canvas->render_target->Size();

        // 1424 * 826
        auto* tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(canvas->GetGameObject());

        {
            auto& canvas_rt = *canvas->GetGameObject()->GetComponent<RectTransform>();
            canvas_rt._local_rect = rect{ vec2{0,0}, vec2{screen_size} };
            canvas_rt._matrix = mat4{ scale(vec3(2.0f / screen_size.x, 2.0f / screen_size.y, 1.0f)) };
        }
        

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

            const auto& parent_rt = *parent->GetComponent<RectTransform>();
            const rect& parent_rect = parent_rt._local_rect;
            vec2 parent_pivot = parent_rt.pivot * parent_rect.size;

            const auto rt_handle = child->GetComponent<RectTransform>();
            if (!rt_handle)
                return false;

            auto& rt = *rt_handle;
            const auto& t = *child->Transform();

            vec2 min = parent_rect.size * rt.anchor_min + rt.offset_min;
            vec2 max = parent_rect.size * rt.anchor_max + rt.offset_max;
            rt._local_rect.position = min - parent_pivot;
            rt._local_rect.size = max - min;

            vec2 pivot_pt = rt._local_rect.position + rt.pivot * rt._local_rect.size;
            rt._matrix = parent_rt._matrix *
                translate(vec3(pivot_pt, 0)) *
                mat4 { quat_cast<mat3>(t.rotation) *
                scale(t.scale) } *
                translate(vec3(rt._local_rect.position + vec2(0.5f) * rt._local_rect.size - pivot_pt, 0));

            return true;
        });
    }

}
