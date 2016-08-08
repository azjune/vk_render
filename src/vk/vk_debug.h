//
// Created by ld016 on 16-8-8.
//

#ifndef APP_VK_DEBUG_H
#define APP_VK_DEBUG_H

#include <vulkan/vulkan.hpp>
#include <iostream>

class vk_debug {
public:
    void init(VkInstance instance);
    void deInit();

    std::vector<const char*> validationLayerNames = {"VK_LAYER_LUNARG_standard_validation"};

    static VkBool32 debugCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t obj,
            size_t location,
            int32_t code,
            const char* layerPrefix,
            const char* msg,
            void* userData);

private:
    VkInstance _instance{VK_NULL_HANDLE};
    VkDebugReportCallbackEXT _callback;
};


#endif //APP_VK_DEBUG_H
