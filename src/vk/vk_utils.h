//
// Created by ld016 on 16-8-9.
//

#ifndef APP_VK_UTILS_HPP
#define APP_VK_UTILS_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

namespace vk_utils{
    const std::vector<const char*> validationLayerNames = {
            "VK_LAYER_LUNARG_standard_validation"
    };


    inline void checkVulkanResult( VkResult &result,const char *msg ) {
        if(result != VK_SUCCESS) {
            throw std::runtime_error(msg);
        }
    }

#ifdef VK_USE_PLATFORM_XCB_KHR
    VkSurfaceKHR createSurface(VkInstance instance,xcb_connection_t* connection,xcb_window_t window);
#elif VK_USE_PLATFORM_WIN32_KHR
    VkSurfaceKHR createSurface(VkInstance instance,HINSTANCE hinstance,HWND hwnd);
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

    VkInstance createInstance(const char* appName = "app", const char* engineName = nullptr, bool enabledValidationLayer = false);

    VkDevice createDevice(VkPhysicalDevice physicalDevice,uint32_t familyIndex,bool enabledValidationLayer = false);

    VkSwapchainKHR createSwapChain(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface,VkDevice device);

    std::vector<VkImageView> createImageViews(VkDevice device,std::vector<VkImage>& images);

    VkPhysicalDevice getPhysicalDevice(uint32_t& queueFamilyIndex, VkInstance instance,VkSurfaceKHR surface);

    std::vector<VkImage> getSwapchainImages(VkDevice device,VkSwapchainKHR swapchain);

    uint32_t findQueueFamilyIndex(VkPhysicalDevice& physicalDevice);
}

#endif //APP_VK_UTILS_HPP
