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

        template<typename T>
        void FocusConfig()
        {
            ImGui::SetWindowFocus(window_name);
            is_open = true;
            if constexpr (std::is_same_v<T, TagManager>) _selection = _tags_and_layers;
            if constexpr (std::is_same_v<T, SceneManager>) _selection = _scene;
            if constexpr (std::is_same_v<T, mono::ScriptSystem>) _selection = _script;
            if constexpr (std::is_same_v<T, PhysicsSystem>) _selection = _physics;
        }

    private:
        enum config
        {
            _tags_and_layers = 0,
            _scene,
            _script,
            _physics,
            _max
        };

        int _selection = 0;
    };
}