//
// Created by ld016 on 16-8-11.
//

#ifndef APP_V_PLATFORM_HPP
#define APP_V_PLATFORM_HPP

#if defined(_WIN32)

#define VK_USE_PLATFORM_WIN32_KHR 1

#elif defined(__linux)

#define VK_USE_PLATFORM_XCB_KHR 1

#else

#error Platform not yet supported

#endif

#include <vulkan/vulkan.h>

#endif //APP_V_PLATFORM_HPP
