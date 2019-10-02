#pragma once

#include <app/GamepadSystem.h>
#include <Windows.h>
#include <Xinput.h>

namespace idk::win
{

    class XInputSystem : public GamepadSystem
    {
    public:
        void Init();
        void Shutdown();
        void Update();

        bool GetButtonDown(char player, GamepadButton button);
        bool GetButtonUp(char player, GamepadButton button);
        bool GetButton(char player, GamepadButton button);
        float GetAxis(char player, GamepadAxis axis);

    private:
        using Buffer = XINPUT_STATE[4];
        Buffer _buffers[2];
        char _curr_buffer_index = 0;
        char _connected_users = 0;

        Buffer& _curr_buf() { return _buffers[_curr_buffer_index]; }
        Buffer& _prev_buf() { return _buffers[!_curr_buffer_index]; }
        void _swap_bufs() { _curr_buffer_index = !_curr_buffer_index; }
    };

}