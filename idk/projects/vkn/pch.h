// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef VKNPCH_H
#define VKNPCH_H
#include <idk.h>

// add headers that you want to pre-compile here
#include "framework.h"
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
#include <Core/Core.h>
#include <vkn/ManagedVulkanObjs.h>
#endif //PCH_H
