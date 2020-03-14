#pragma once

#include <idk.h>
#include <app/Application.h>
#include <iostream>

using idk::string;
class TestApplication :
    public idk::Application
{
public:
    int i = 0;
    void Init() override { i++;  std::cout << "application init\n"; }
    void Shutdown() override { i++;  std::cout << "application shutdown\n"; }

    // app
    void PollEvents() override { ++i; }
    string Exec(std::string_view path, idk::span<const char*> argv, bool wait) override { };

    // movement
    idk::vec2 GetMouseScreenPos() override { return idk::vec2{}; };
    idk::vec2 GetMouseScreenDel() override { return idk::vec2{}; };
    idk::ivec2 GetMousePixelPos() override { return idk::ivec2{}; };
    idk::ivec2 GetMousePixelDel() override { return idk::ivec2{}; };
    idk::ivec2 GetMouseScroll() override { return {}; }
    bool GetKeyDown(idk::Key) override { return false; };
    bool GetKey(idk::Key) override { return false; };
    bool GetKeyUp(idk::Key) override { return false; };
    char GetChar() override { return 0; }

    // windows
    idk::ivec2 GetScreenSize() override { return idk::ivec2{}; }
    bool SetFullscreen(bool) override { return false; };
    bool SetScreenSize(idk::ivec2) override { return false; };

    // file IO
    string GetExecutableDir() { return {}; };
    string GetAppData() { return {}; };
    string GetCurrentWorkingDir() { return {}; };
    std::optional<string> OpenFileDialog(const idk::DialogOptions&) override { return {}; };

    idk::vector<idk::Device> GetNetworkDevices() { return{}; };
};