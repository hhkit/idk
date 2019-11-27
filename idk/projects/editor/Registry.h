#pragma once

#include <idk.h>

namespace idk
{
    class Registry
    {
    public:
        Registry(string_view mount_path) : mount_path{ mount_path } {}
        string get(string_view key);
        void set(string_view key, string value);
    private:
        string mount_path;
    };
}