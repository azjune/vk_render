//
// Created by ld016 on 16-8-11.
//

#include "v_renderer.h"
#include "v_window.h"
#include <iostream>
#include <assert.h>
#include <string.h>

v_renderer::v_renderer() {
    _initInstance();
    _initDebug();
    _initDevice();
}

v_renderer::~v_renderer() {
    delete _window;
    _deInitDevice();
    _deInitDebug();
    _deInitInstance();
}

v_window* v_renderer::openWindow(uint32_t width, uint32_t height, const char *title) {
    _window = new v_window(this,width,height,title);

    return _window;
}

bool v_renderer::run() {
    if(nullptr != _window){
        return _window->update();
    }
    return true;
}

void v_renderer::_initInstance() {
    VkApplicationInfo applicationInfo = {};
    {
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.pApplicationName = "app";
        applicationInfo.pEngineName = "engine";
    }

    std::vector<const char*> extensions;
    std::vector<VkExtensionProperties> property_list;
    {
        uint32_t count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr,&count, nullptr);
        property_list.resize(count);
        vkEnumerateInstanceExtensionProperties(nullptr,&count, property_list.data());
        for(auto& property : property_list){
            extensions.push_back(property.extensionName);
        }
    }

    VkInstanceCreateInfo createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = _instance_layer.size();
        createInfo.ppEnabledLayerNames = _instance_layer.data();
    }

    auto result = vkCreateInstance(&createInfo, nullptr,&_instance);
    assert(result==VK_SUCCESS && "create instance faild");
}

void v_renderer::_deInitInstance() {
    vkDestroyInstance(_instance, nullptr);
    _instance = VK_NULL_HANDLE;
}

void v_renderer::_initDevice() {
    {
        uint32_t gpu_count = 0;
        vkEnumeratePhysicalDevices(_instance,&gpu_count, nullptr);
        std::vector<VkPhysicalDevice> gpu_list( gpu_count );
        vkEnumeratePhysicalDevices(_instance,&gpu_count, gpu_list.data());
        _gpu = gpu_list[0];
    }
    {
        uint32_t familyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(_gpu,&familyCount, nullptr);
        std::vector<VkQueueFamilyProperties> family_property_list(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(_gpu,&familyCount, family_property_list.data());

        bool found = false;
        for(uint32_t i = 0;i<familyCount;i++){
            if(family_property_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                _graphics_family_index = i;
                found = true;
            }
        }
        assert(found && "Queue family supporting graphics not found.");
    }


    VkDeviceQueueCreateInfo queueCreateInfo = {};
    {
        float queuePriorities[] = {1.0f};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfo.queueFamilyIndex = _graphics_family_index;
    }

    std::vector<const char*> extensions;
    std::vector<VkExtensionProperties> property_list;
    {
        uint32_t count = 0;
        vkEnumerateDeviceExtensionProperties(_gpu, nullptr,&count, nullptr);
        property_list.resize(count);
        vkEnumerateDeviceExtensionProperties(_gpu, nullptr,&count, property_list.data());
        for(auto& property : property_list){
            extensions.push_back(property.extensionName);
        }
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    {
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = extensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    }
    vkCreateDevice(_gpu,&deviceCreateInfo, nullptr,&_device);
}

void v_renderer::_deInitDevice() {
    vkDestroyDevice(_device, nullptr);
    _device = VK_NULL_HANDLE;
}

#ifdef _DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugReportFlagsEXT		flags,
        VkDebugReportObjectTypeEXT	obj_type,
        uint64_t					src_obj,
        size_t						location,
        int32_t						msg_code,
        const char *				layer_prefix,
        const char *				msg,
        void *						user_data
)
{
    std::cerr << "VKDBG: ";
    if( flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) {
        std::cerr << "INFO: ";
    }
    if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
        std::cerr << "WARNING: ";
    }
    if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
        std::cerr << "PERFORMANCE: ";
    }
    if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
        std::cerr << "ERROR: ";
    }
    if( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
        std::cerr << "DEBUG: ";
    }
    std::cerr << "@[" << layer_prefix << "]: ";
    std::cerr << msg << std::endl;

    return false;
}

PFN_vkCreateDebugReportCallbackEXT createDebugReportCallback = nullptr;
PFN_vkDestroyDebugReportCallbackEXT destroyDebugReportCallback = nullptr;

void v_renderer::_initDebug() {
    createDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance,"vkCreateDebugReportCallbackEXT");
    destroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance,"vkDestroyDebugReportCallbackEXT");

    assert((createDebugReportCallback && destroyDebugReportCallback) && "Vulkan ERROR: Can't fetch debug function pointers.");

    VkDebugReportCallbackCreateInfoEXT createInfoEXT = {};
    {
        createInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        createInfoEXT.pfnCallback = &debugCallback;
        createInfoEXT.flags =
                VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                VK_DEBUG_REPORT_WARNING_BIT_EXT |
                VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                VK_DEBUG_REPORT_ERROR_BIT_EXT |
                VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    }

    createDebugReportCallback(_instance,&createInfoEXT, nullptr,&_debugReportCallback);
}

void v_renderer::_deInitDebug() {
    destroyDebugReportCallback(_instance,_debugReportCallback, nullptr);
    _debugReportCallback = VK_NULL_HANDLE;
}

#else
void v_renderer::_initDebug() {}
void v_renderer::_deInitDebug(){}
#endif




