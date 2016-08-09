//
// Created by ld016 on 16-8-9.
//

#ifndef APP_VK_UTILS_HPP
#define APP_VK_UTILS_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

namespace vk_utils{
    inline void checkVulkanResult( VkResult &result,const char *msg ) {
        if(result != VK_SUCCESS) {
            throw std::runtime_error(msg);
        }
    }


#ifdef VK_USE_PLATFORM_XCB_KHR
    void createSurface(VkInstance instance,VkSurfaceKHR* pSurface,xcb_connection_t* connection,xcb_window_t window);
#elif VK_USE_PLATFORM_WIN32_KHR
    void createSurface(VkInstance instance,VkSurfaceKHR* pSurface,HINSTANCE hinstance,HWND hwnd);
#endif

    struct vk_window{
        unsigned width;
        unsigned height;
        const char* title = nullptr;
#ifdef VK_USE_PLATFORM_XCB_KHR
        xcb_connection_t* connection = nullptr;
        xcb_window_t window = 0;
#elif VK_USE_PLATFORM_WIN32_KHR
        HINSTANCE hinstance = 0;
        HWND hwnd = 0;
#endif
    };

    vk_window* createWindow(unsigned width, unsigned height, const char* title);

    void getPhysicalDevice(VkPhysicalDevice& physicalDevice, VkInstance& instance,VkSurfaceKHR& surface,uint32_t queueFamilyIndex);
}

#endif //APP_VK_UTILS_HPP
