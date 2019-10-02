#pragma once

#include <core/ISystem.h>
#include <app/GamepadEnums.h>

namespace idk
{
    class GamepadSystem : public ISystem
    {
    public:
        virtual void Init() = 0;
        virtual void Shutdown() = 0;
        virtual void Update() = 0;

        virtual bool GetButtonDown(char, GamepadButton) = 0;
        virtual bool GetButtonUp(char, GamepadButton) = 0;
        virtual bool GetButton(char, GamepadButton) = 0;
        virtual float GetAxis(char, GamepadAxis) = 0;
        virtual void SetRumble(char, float low_freq, float high_freq) = 0;
    };
}