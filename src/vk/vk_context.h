//
// Created by ld016 on 16-8-8.
//

#ifndef APP_VK_CONTEXT_H
#define APP_VK_CONTEXT_H

#include <vulkan/vulkan.h>
#include "vk_utils.h"
#include "vk_debug.h"

class vk_context {
public:
    vk_context(vk_utils::vk_window* window);
    ~vk_context();

    void initConstance();
    void initSurface();
    void initDevice();

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface;

    uint32_t familyIndex;
private:
    vk_debug _debug;



};


#endif //APP_VK_CONTEXT_H
