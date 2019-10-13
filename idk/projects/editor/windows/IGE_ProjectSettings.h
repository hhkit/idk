#pragma once
#include <editor/windows/IGE_IWindow.h>

namespace idk
{
    class IGE_ProjectSettings : public IGE_IWindow
    {
    public:
        IGE_ProjectSettings();

        virtual void BeginWindow() override;
        virtual void Update() override;

    private:
        int _selection = 0;
    };
}