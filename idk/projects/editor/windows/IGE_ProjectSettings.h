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
        void Focus()
        {
            if constexpr (std::is_same_v<T, TagSystem>)
                _selection = _tags_and_layers;
        }

    private:
        enum config
        {
            _tags_and_layers = 0,
            _max
        };

        int _selection = 0;
    };
}