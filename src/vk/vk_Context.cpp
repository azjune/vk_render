//
// Created by ld016 on 16-8-5.
//

#include "vk_Context.h"

vk_Context::vk_Context() {

}

vk_Context::~vk_Context() {
    instance.destroy();
}

void vk_Context::creatInstance(const char* appName, const char* engineName) {
    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = appName;
    applicationInfo.pEngineName = engineName;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> extensions;
    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

#if ENABLE_VALIDATION_LAYERS
    extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    vk::InstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    instance = vk::createInstance(instanceCreateInfo);
}