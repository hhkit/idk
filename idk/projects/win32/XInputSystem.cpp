#include "pch.h"
#include "XInputSystem.h"
#include <idk.h>

namespace idk::win
{

    // number of seconds between each check for new player
    constexpr static auto check_for_new_gamepad_delay = 3.0f;

    void XInputSystem::Init()
    {
        ZeroMemory(&_buffers, sizeof(_buffers));

        DWORD dwResult;
        for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
        {
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            // Simply get the state of the controller from XInput.
            dwResult = XInputGetState(i, &state);

            if (dwResult == ERROR_SUCCESS)
            {
                // Controller is connected 
                _curr_buf()[i] = _prev_buf()[i] = state;
                _connected_users |= 1 << i;
            }
            else
            {
                // Controller is not connected 
            }
        }
    }

    void XInputSystem::Shutdown()
    {
    }

    void XInputSystem::Update()
    {
        _swap_bufs();

        DWORD dwResult;
        for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
        {
            if ((_connected_users & (1 << i)) == 0)
                continue;

            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            // Simply get the state of the controller from XInput.
            dwResult = XInputGetState(i, &state);

            if (dwResult == ERROR_SUCCESS)
            {
                // Controller is connected 
                _curr_buf()[i] = state;
            }
            else
            {
                // Controller is not connected 
                _connected_users &= ~(1 << i);
            }
        }
    }

    bool XInputSystem::GetButtonDown(char player, GamepadButton button)
    {
        return (_prev_buf()[player].Gamepad.wButtons & static_cast<unsigned short>(button)) == 0 &&
               (_curr_buf()[player].Gamepad.wButtons & static_cast<unsigned short>(button)) != 0;
    }

    bool XInputSystem::GetButtonUp(char player, GamepadButton button)
    {
        return (_prev_buf()[player].Gamepad.wButtons & static_cast<unsigned short>(button)) != 0 &&
               (_curr_buf()[player].Gamepad.wButtons & static_cast<unsigned short>(button)) == 0;
    }

    bool XInputSystem::GetButton(char player, GamepadButton button)
    {
        return (_curr_buf()[player].Gamepad.wButtons & static_cast<unsigned short>(button)) != 0;
    }

    float XInputSystem::GetAxis(char player, GamepadAxis axis)
    {
        // todo: handle deadzones
        switch (axis)
        {
        case GamepadAxis::LeftStickX:
            return std::clamp(static_cast<float>(_curr_buf()[player].Gamepad.sThumbLX) / SHRT_MAX, -1.0f, 1.0f);
            break;
        case GamepadAxis::LeftStickY:
            return std::clamp(static_cast<float>(_curr_buf()[player].Gamepad.sThumbLY) / SHRT_MAX, -1.0f, 1.0f);
            break;
        case GamepadAxis::RightStickX:
            return std::clamp(static_cast<float>(_curr_buf()[player].Gamepad.sThumbRX) / SHRT_MAX, -1.0f, 1.0f);
            break;
        case GamepadAxis::RightStickY:
            return std::clamp(static_cast<float>(_curr_buf()[player].Gamepad.sThumbRY) / SHRT_MAX, -1.0f, 1.0f);
            break;
        case GamepadAxis::LeftTrigger:
            return std::clamp(static_cast<float>(_curr_buf()[player].Gamepad.bLeftTrigger) / SHRT_MAX, -1.0f, 1.0f);
            break;
        case GamepadAxis::RightTrigger:
            return std::clamp(static_cast<float>(_curr_buf()[player].Gamepad.bRightTrigger) / SHRT_MAX, -1.0f, 1.0f);
            break;
        default:
            return 0;
            break;
        }
    }

}