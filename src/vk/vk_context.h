//
// Created by ld016 on 16-8-8.
//

#ifndef APP_VK_CONTEXT_H
#define APP_VK_CONTEXT_H

#include <vulkan/vulkan.h>
#include "vk_debug.h"

class vk_context {
public:
    vk_context();
    ~vk_context();

    VkInstance instance;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

private:
    vk_debug _debug;

    void _initConstance();

    void _initDevice();

};


#endif //APP_VK_CONTEXT_H
