#include "pch.h"
#include "XInputSystem.h"
#include <idk.h>
#include <Windows.h>
#include <Xinput.h>
#include <core/Core.h>

#undef max
#if DEBUG_XINPUT
#include <fstream>
#include <iostream>
#include <bitset>
#endif
namespace idk::win
{

    // number of seconds between each check for new player
    constexpr static auto check_for_new_connection_delay = 3.0f;
#if DEBUG_XINPUT
    static std::ofstream log;
    std::ostream& logger()  
    {
        std::ostream& ret = log ? log : std::cout;
        return ret;
    }
#endif
    void XInputSystem::Init()
    {
#if DEBUG_XINPUT
        AllocConsole();
        FILE* result;
        freopen_s(&result,"CONOUT$", "w", stdout);
        //log = std::ofstream{"xinput.log"};
        logger() << "Init"<<std::endl;
#endif
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
                _connected_users &= ~(1 << i);
                _curr_buf()[i] = _prev_buf()[i] = x_gamepad_state{ 0, 0, 0, 0, 0, 0, 0 };
            }
        }

        _new_connection_check_timer = check_for_new_connection_delay;
    }

    void XInputSystem::Shutdown()
    {
    }
#pragma optimize("",off)
    void XInputSystem::Update()
    {
        _swap_bufs();

        _new_connection_check_timer -= Core::GetRealDT().count();
        const bool check_for_new_connection = _new_connection_check_timer <= 0;
        if (check_for_new_connection)
            _new_connection_check_timer = check_for_new_connection_delay;
        DWORD dwResult;
        for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
        {
            if (!check_for_new_connection)
            {
                if ((_connected_users & (1 << i)) == 0)
                    continue;
            }

            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            // Simply get the state of the controller from XInput.
            dwResult = XInputGetState(i, &state);

            if (dwResult == ERROR_SUCCESS)
            {
                // Controller is connected 
                _curr_buf()[i] = _process_state(state);
                _connected_users |= 1 << i;
            }
            else
            {
                // Controller is not connected
                _connected_users &= ~(1 << i);
                _curr_buf()[i] = _prev_buf()[i] = x_gamepad_state{ 0, 0, 0, 0, 0, 0, 0 };
            }
            if (_curr_buf()[i].buttons)
            {
                auto& var = _curr_buf()[i].buttons;
#if DEBUG_XINPUT
                std::bitset<sizeof(var) * 8> bitfield{ var };
                logger() <<"Player["<<i<<"]buttons: "<<bitfield<< std::endl;
#endif 
            }
            else
            {
#if DEBUG_XINPUT
                logger() <<"Player["<<i<<"]" << " No buttons pressed" << std::endl;
#endif
            }
        }
    }

    bool XInputSystem::GetButtonDown(char player, GamepadButton button) const
    {
        return (_prev_buf()[player].buttons & static_cast<unsigned short>(button)) == 0 &&
               (_curr_buf()[player].buttons & static_cast<unsigned short>(button)) != 0;
    }

    bool XInputSystem::GetButtonUp(char player, GamepadButton button) const
    {
        return (_prev_buf()[player].buttons & static_cast<unsigned short>(button)) != 0 &&
               (_curr_buf()[player].buttons & static_cast<unsigned short>(button)) == 0;
    }

    bool XInputSystem::GetButton(char player, GamepadButton button) const
    {
        return (_curr_buf()[player].buttons & static_cast<unsigned short>(button)) != 0;
    }

    float XInputSystem::GetAxis(char player, GamepadAxis axis) const
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

    char XInputSystem::GetConnectedPlayers() const
    {
        return _connected_users;
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