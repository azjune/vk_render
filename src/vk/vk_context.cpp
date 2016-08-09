//
// Created by ld016 on 16-8-8.
//

#include "vk_context.h"

vk_context::vk_context(vk_utils::vk_window* window) {
    initConstance();
    _debug.init(instance);
    initDevice();
}

vk_context::~vk_context() {
    vkDestroyDevice(device, nullptr);
    _debug.deInit();
    vkDestroyInstance(instance, nullptr);
}

void vk_context::initConstance() {
    VkApplicationInfo applicationInfo = {};
    {
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.pApplicationName = "app";
        applicationInfo.pEngineName = "engine";
    }

    VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        std::vector<const char*> layerNames = _debug.validationLayerNames;

        std::vector<const char*> extensionNames;
            uint32_t instanceExtensionPropertiesCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr,&instanceExtensionPropertiesCount, nullptr);
            std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionPropertiesCount);
            vkEnumerateInstanceExtensionProperties(nullptr,&instanceExtensionPropertiesCount, instanceExtensionProperties.data());
            for(auto& ex : instanceExtensionProperties){
                extensionNames.push_back(ex.extensionName);
            }

        instanceCreateInfo.enabledLayerCount = layerNames.size();
        instanceCreateInfo.ppEnabledLayerNames = layerNames.data();
        instanceCreateInfo.enabledExtensionCount = extensionNames.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

    vkCreateInstance(&instanceCreateInfo, nullptr,&instance);
    assert(instance);
}

void vk_context::initSurface() {

}

void vk_context::initDevice() {
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance,&physicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance,&physicalDeviceCount, physicalDevices.data());
        physicalDevice = physicalDevices[0];
        VkBool32 isSuppored = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,familyIndex,surface,&isSuppored);
    }

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice,&features);

    {
        uint32_t deviceQueueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&deviceQueueFamilyPropertiesCount, nullptr);
        std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyPropertiesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&deviceQueueFamilyPropertiesCount, deviceQueueFamilyProperties.data());
        for(uint32_t i = 0;i<deviceQueueFamilyProperties.size();i++){
            if(deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                familyIndex = i;
            }
        }
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
    {
        float priorities = 1.0f;
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.pQueuePriorities = &priorities;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.queueFamilyIndex = familyIndex;
    }

    std::vector<const char*> extensionNames = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo = {};
    {
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pEnabledFeatures           = &features;
        deviceCreateInfo.queueCreateInfoCount       = 1;
        deviceCreateInfo.pQueueCreateInfos          = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledLayerCount          = _debug.validationLayerNames.size();
        deviceCreateInfo.ppEnabledLayerNames        = _debug.validationLayerNames.data();
        deviceCreateInfo.enabledExtensionCount      = extensionNames.size();
        deviceCreateInfo.ppEnabledExtensionNames    = extensionNames.data();
    }

    vkCreateDevice(physicalDevice,&deviceCreateInfo, nullptr,&device);

    vkGetDeviceQueue(device,familyIndex,0,&graphicsQueue);

    assert(device);
    assert(graphicsQueue);
}

