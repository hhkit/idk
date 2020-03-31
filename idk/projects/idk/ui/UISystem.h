#pragma once

#include <idk.h>
#include <core/ISystem.h>
#include <res/Guid.inl>

namespace idk
{
    class UISystem : public ISystem
    {
    public:
        constexpr static Guid default_material_inst{ 0x90da4f5c, 0x0453, 0x4e77, 0xbb3fb506c067d085 };

        virtual void Init() override {};
        virtual void LateInit() override;
        virtual void Shutdown() override {};

        void Update(span<class Canvas>);
        void FinalizeMatrices(span<class Canvas>);
        void RecalculateRects(Handle<class RectTransform>);

        Handle<class Canvas> FindCanvas(Handle<GameObject> go) const;

    private:
        void ComputeCanvasHierarchyRects(Handle<Canvas> canvas);
        void FinalizeMatrices(Handle<Canvas> canvas);
    };
    void calc_rt(idk::Handle<idk::GameObject>& child, const idk::Handle<idk::RectTransform>& rt_handle);
}