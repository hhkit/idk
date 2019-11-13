#pragma once

#include <idk.h>
#include <core/ISystem.h>

namespace idk
{
    class UISystem : public ISystem
    {
    public:
        virtual void Init() override {};
        virtual void Shutdown() override {};
        virtual void Update(span<class Canvas>);

        void ComputeCanvasHierarchyRects(Handle<Canvas> canvas);
    };
}