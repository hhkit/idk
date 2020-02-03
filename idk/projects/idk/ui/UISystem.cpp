#include "stdafx.h"
#include "UISystem.h"
#include <app/Application.h>
#include <ui/Canvas.h>
#include <ui/RectTransform.h>
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <scene/SceneManager.h>
#include <ds/slow_tree.inl>
#include <gfx/MaterialInstance.h>
#include <gfx/Camera.h>
#include <gfx/RenderTarget.h>
#include <math/matrix_transforms.inl>
#include <math/quaternion.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <ds/span.inl>
#include <ds/result.inl>

namespace idk
{
    void UISystem::LateInit()
    {
        auto res = Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/ui.frag", false);
        if (!res)
            return;

        auto frag = *res;
        auto mat = Core::GetResourceManager().LoaderCreateResource<Material>(Guid{ 0x90da4f5c, 0x0453, 0x4e77, 0xbb3fb506c067d085 });
        if (mat)
        {
            mat->_shader_program = frag;
            mat->Name("Default UI");
        }
        auto inst = Core::GetResourceManager().LoaderCreateResource<MaterialInstance>(UISystem::default_material_inst);
        if (inst)
        {
            inst->material = mat;
            inst->Name("Default UI");
            mat->_default_instance = inst;
        }
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

    void UISystem::RecalculateRects([[maybe_unused]]Handle<RectTransform> rt)
    {
        //if (!rt)
        //    return;
        //auto go = rt->GetGameObject();
        //if (go->HasComponent<Canvas>())
        //    ComputeCanvasHierarchyRects(rt->GetGameObject()->GetComponent<Canvas>());
        //else
        //{
        //    if (!go->Parent())
        //        return;

        //    // will def have parent, this visit starts from Canvas, which will reach the branch above.
        //    const auto parent = go->Parent();

        //    const auto& parent_rt = *parent->GetComponent<RectTransform>();
        //    const rect& parent_rect = parent_rt._local_rect;
        //    vec2 parent_pivot = parent_rt.pivot * parent_rect.size;

        //    auto& rt = *rt_handle;

        //    vec2 min = parent_rect.size * rt.anchor_min + rt.offset_min;
        //    vec2 max = parent_rect.size * rt.anchor_max + rt.offset_max;
        //    rt._local_rect.position = min - parent_pivot;
        //    rt._local_rect.size = max - min;
        //}
    }

    void UISystem::ComputeCanvasHierarchyRects(Handle<Canvas> canvas)
    {
        // assuming screen space
        auto screen_size = canvas->render_target->Size();

        auto canvas_go = canvas->GetGameObject();

        // 1424 * 826
        if (!canvas_go->HasComponent<RectTransform>())
        {
            LOG_WARNING_TO(LogPool::GAME, "Canvas hierarchy must use RectTransform.");
            return;
        }

        canvas_go->GetComponent<RectTransform>()->_local_rect = rect{ vec2{0,0}, vec2{screen_size} };
        auto* tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(canvas_go);

        tree->visit([canvas](Handle<GameObject> child, int)
        {
            if (child->HasComponent<Canvas>())
            {
                if (child->GetComponent<Canvas>() == canvas)
                    return true;
                else
                {
                    LOG_WARNING_TO(LogPool::GAME, "Canvas cannot contain another Canvas.");
                    return false;
                }
            }

            const auto rt_handle = child->GetComponent<RectTransform>();
            if (!rt_handle)
            {
                LOG_WARNING_TO(LogPool::GAME, "Canvas hierarchy must use RectTransform.");
                return false;
            }

            // will def have parent, this visit starts from Canvas, which will reach the branch above.
            const auto parent = child->Parent();

            const auto& parent_rt = *parent->GetComponent<RectTransform>();
            const rect& parent_rect = parent_rt._local_rect;
            vec2 parent_pivot = parent_rt.pivot * parent_rect.size;

            auto& rt = *rt_handle;

            vec2 min = parent_rect.size * rt.anchor_min + rt.offset_min;
            vec2 max = parent_rect.size * rt.anchor_max + rt.offset_max;
            rt._local_rect.position = min - parent_pivot;
            rt._local_rect.size = max - min;

            return true;
        });
    }

    void UISystem::FinalizeMatrices(Handle<Canvas> canvas)
    {
        // assuming screen space
        auto screen_size = canvas->render_target->Size();

        auto canvas_go = canvas->GetGameObject();

        // 1424 * 826
        if (!canvas_go->HasComponent<RectTransform>())
        {
            LOG_WARNING_TO(LogPool::GAME, "Canvas hierarchy must use RectTransform.");
            return;
        }

        auto& canvas_rt = *canvas_go->GetComponent<RectTransform>();
        canvas_rt._local_rect = rect{ vec2{0,0}, vec2{screen_size} };
        canvas_rt._matrix = mat4{ scale(vec3(2.0f / screen_size.x, 2.0f / screen_size.y, 1.0f)) };

        auto* tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(canvas_go);
        
        tree->visit([canvas](Handle<GameObject> child, int)
        {
            if (child->HasComponent<Canvas>())
            {
                if (child->GetComponent<Canvas>() == canvas)
                    return true;
                else
                {
                    LOG_WARNING_TO(LogPool::GAME, "Canvas cannot contain another Canvas.");
                    return false;
                }
            }

            const auto rt_handle = child->GetComponent<RectTransform>();
            if (!rt_handle)
            {
                LOG_WARNING_TO(LogPool::GAME, "Canvas hierarchy must use RectTransform.");
                return false;
            }

            auto& rt = *rt_handle;
            const auto& t = *child->Transform();

            // will def have parent, this visit starts from Canvas, which will reach the branch above.
            const auto parent = child->Parent();

            const auto& parent_rt = *parent->GetComponent<RectTransform>();
            const rect& parent_rect = parent_rt._local_rect;
            vec2 parent_pivot = parent_rt.pivot * parent_rect.size;

            vec2 min = parent_rect.size * rt.anchor_min + rt.offset_min;
            vec2 max = parent_rect.size * rt.anchor_max + rt.offset_max;
            rt._local_rect.position = min - parent_pivot;
            rt._local_rect.size = max - min;

            vec2 pivot_pt = rt._local_rect.position + rt.pivot * rt._local_rect.size;
            rt._matrix = parent_rt._matrix *
                translate(vec3(pivot_pt, t.position.z)) *
                quat_cast<mat4>(t.rotation) *
                scale(t.scale) *
                translate(vec3(rt._local_rect.position + vec2(0.5f) * rt._local_rect.size - pivot_pt, 0));

            return true;
        });
    }



    Handle<class Canvas> UISystem::FindCanvas(Handle<GameObject> go) const
    {
        auto parent = go->Parent();
        while (parent)
        {
            if (const auto canvas = parent->GetComponent<Canvas>())
                return canvas;
            parent = parent->Parent();
        }
        return {};
    }

}
