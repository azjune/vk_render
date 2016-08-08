//
// Created by ld016 on 16-8-8.
//

#include "vk_debug.h"

VkBool32 vk_debug::debugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData) {

    std::cerr << "VULKAN : ";
    switch(flags){
        case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
            std::cerr << "INFORMATION : ";
            break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
            std::cerr << "WARNING : ";
            break;
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
            std::cerr << "PERFORMANCE : ";
            break;
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
            std::cerr << "ERROR : ";
            break;
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
            std::cerr << "DEBUG : ";
            break;
    }
    std::cerr << msg << std::endl;

    return VK_FALSE;
}

void vk_debug::deInit() {
    if(VK_NULL_HANDLE == _instance)
        return;
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");
    if(nullptr != func){
        func(_instance,_callback, nullptr);
    }
}

void vk_debug::init(VkInstance instance) {
    _instance = instance;

    VkDebugReportCallbackCreateInfoEXT createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
                       VK_DEBUG_REPORT_ERROR_BIT_EXT |
                       VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                       VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                       VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    createInfo.pfnCallback = debugCallback;

    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
    if(nullptr != func){
        func(_instance,&createInfo, nullptr,&_callback);
    }
}

