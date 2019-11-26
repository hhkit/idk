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

		virtual bool GetButtonDown(char, GamepadButton) const { return false; };
		virtual bool GetButtonUp(char, GamepadButton) const { return false; };
		virtual bool GetButton(char, GamepadButton) const { return false; };
		virtual float GetAxis(char, GamepadAxis) const { return false; };
		virtual void SetRumble(char player, float low_freq, float high_freq) { (player); (low_freq); (high_freq); };

        // returns bitmask of connected players
        virtual char GetConnectedPlayers() const { return 0; }
    };
}