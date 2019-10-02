#pragma once

#include <core/ISystem.h>
#include <app/GamepadEnums.h>

namespace idk
{
    class GamepadSystem : public ISystem
    {
    public:
		virtual void Init() {};
		virtual void Shutdown() {};
		virtual void Update() {};

		virtual bool GetButtonDown(char, GamepadButton) { return false; };
		virtual bool GetButtonUp(char, GamepadButton) { return false; };
		virtual bool GetButton(char, GamepadButton) { return false; };
		virtual float GetAxis(char, GamepadAxis) { return false; };
    };
}