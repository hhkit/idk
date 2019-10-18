#pragma once

#include <idk.h>
#include <app/Application.h>
#include <iostream>

class TestApplication :
    public idk::Application
{
public:
    int i = 0;
    void Init() override { i++;  std::cout << "application init\n"; }
    void PollEvents() override { ++i; }
    idk::vec2 GetMouseScreenPos() override { return idk::vec2{}; };
    idk::vec2 GetMouseScreenDel() override { return idk::vec2{}; };
    idk::ivec2 GetMousePixelPos() override { return idk::ivec2{}; };
    idk::ivec2 GetMousePixelDel() override { return idk::ivec2{}; };
    idk::ivec2 GetMouseScroll() override { return {}; }
    bool GetKeyDown(idk::Key) override { return false; };
    bool GetKey(idk::Key) override { return false; };
    bool GetKeyUp(idk::Key) override { return false; };
    char GetChar() override { return 0; }
    idk::ivec2 GetScreenSize() override { return idk::ivec2{}; }

    // windows
    bool SetFullscreen(bool) override { return false; };
    bool SetScreenSize(idk::ivec2) override { return false; };

    std::string GetExecutableDir() { return {}; };
    std::string GetAppData() { return {}; };
    std::string GetCurrentWorkingDir() { return {}; };
    std::optional<std::string> OpenFileDialog(const idk::DialogOptions&) override { return {}; };

    void Shutdown() override { i++;  std::cout << "application shutdown\n"; }
};