//
// Created by ld016 on 16-8-30.
//

#include "VContext.h"

void VContext::_initInstance() {
    vk::ApplicationInfo applicationInfo;
    {
        applicationInfo.apiVersion = VK_API_VERSION_1_0;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.pApplicationName = "app";
        applicationInfo.pEngineName = "engine";
    };

    std::vector<vk::ExtensionProperties> extensions;
    std::vector<const char*> extensionNames;
    {
        uint32_t count = 0;
        vk::enumerateInstanceExtensionProperties(nullptr,&count, nullptr);
        extensions.resize(count);
        vk::enumerateInstanceExtensionProperties(nullptr,&count, extensions.data());
        extensionNames.resize(extensions.size());
        for(auto i = 0;i<extensionNames.size();i++){
            extensionNames[i] = extensions[i].extensionName;
        }
    }

    std::vector<const char*> layers = {"VK_LAYER_LUNARG_standard_validation"};

    vk::InstanceCreateInfo createInfo;
    {
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensionNames.data();
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();
    };
    instance = vk::createInstance(createInfo);
}

void VContext::_initPhysicalDevice() {
    auto physicalDevices = instance.enumeratePhysicalDevices();
    for (int i = 0; i <physicalDevices.size(); ++i) {
        
    }
}



