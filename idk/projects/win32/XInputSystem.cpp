#include "pch.h"
#include "XInputSystem.h"
#include <idk.h>
#include <Windows.h>
#include <Xinput.h>
#undef max

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
                _curr_buf()[i] = _prev_buf()[i] = _process_state(state);
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
                _curr_buf()[i] = _process_state(state);
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
        return (_prev_buf()[player].buttons & static_cast<unsigned short>(button)) == 0 &&
               (_curr_buf()[player].buttons & static_cast<unsigned short>(button)) != 0;
    }

    bool XInputSystem::GetButtonUp(char player, GamepadButton button)
    {
        return (_prev_buf()[player].buttons & static_cast<unsigned short>(button)) != 0 &&
               (_curr_buf()[player].buttons & static_cast<unsigned short>(button)) == 0;
    }

    bool XInputSystem::GetButton(char player, GamepadButton button)
    {
        return (_curr_buf()[player].buttons & static_cast<unsigned short>(button)) != 0;
    }

    float XInputSystem::GetAxis(char player, GamepadAxis axis)
    {
        switch (axis)
        {
        case GamepadAxis::LeftStickX: return _curr_buf()[player].thumb_left_x;
        case GamepadAxis::LeftStickY: return _curr_buf()[player].thumb_left_y;
        case GamepadAxis::RightStickX: return _curr_buf()[player].thumb_right_x;
        case GamepadAxis::RightStickY: return _curr_buf()[player].thumb_right_y;
        case GamepadAxis::LeftTrigger: return _curr_buf()[player].left_trigger;
        case GamepadAxis::RightTrigger: return _curr_buf()[player].right_trigger;
        default: return 0;
        }
    }

    void XInputSystem::SetRumble(char player, float low_freq, float high_freq)
    {
        if ((_connected_users & (1 << player)) == 0)
            return;

        XINPUT_VIBRATION vibration;
        ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
        vibration.wLeftMotorSpeed = static_cast<WORD>(std::clamp(low_freq, 0.0f, 1.0f) * 0xffff);
        vibration.wRightMotorSpeed = static_cast<WORD>(std::clamp(high_freq, 0.0f, 1.0f) * 0xffff);
        XInputSetState(player, &vibration);
    }

    static vec2 thumbstick_deadzone_filter(short x, short y, short deadzone)
    {
        vec2 ret{ 0, 0 };
        float mag = sqrtf(static_cast<float>(x) * x + static_cast<float>(y) * y);

        //check if the controller is outside a circular dead zone
        if (mag > deadzone)
        {
            if (mag > SHRT_MAX) mag = SHRT_MAX;
            mag -= deadzone;
            float scale = mag / (SHRT_MAX - deadzone);
            ret.x = std::max((static_cast<float>(x) / SHRT_MAX) * scale, -1.0f);
            ret.y = std::max((static_cast<float>(y) / SHRT_MAX) * scale, -1.0f);
        }

        return ret;
    }

    static float trigger_deadzone_filter(unsigned char val)
    {
        if (val > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
            return std::max(static_cast<float>(val - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (UCHAR_MAX - XINPUT_GAMEPAD_TRIGGER_THRESHOLD), -1.0f);
        else
            return 0;
    }

    XInputSystem::x_gamepad_state XInputSystem::_process_state(const _XINPUT_STATE& state)
    {
        x_gamepad_state ret;
        ret.buttons = state.Gamepad.wButtons;
        ret.left_trigger = trigger_deadzone_filter(state.Gamepad.bLeftTrigger);
        ret.right_trigger = trigger_deadzone_filter(state.Gamepad.bRightTrigger);
        reinterpret_cast<vec2&>(ret.thumb_left_x) = thumbstick_deadzone_filter(
            state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        reinterpret_cast<vec2&>(ret.thumb_right_x) = thumbstick_deadzone_filter(
            state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        return ret;
    }

}