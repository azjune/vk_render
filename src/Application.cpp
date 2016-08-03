//
// Created by ld016 on 16-8-2.
//

#include "Application.h"
#include <vector>
#include <set>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const std::vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkResult CreateDebugReportCallbackEXT(VkInstance instance,
                                      const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugReportCallbackEXT* pCallback) {
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(VkInstance instance,
                                   VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

std::vector<const char*> getRequiredExtensions() {
    std::vector<const char*> extensions;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

#ifdef ENABLE_VALIDATION_LAYERS
    extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    return extensions;
}

bool checkValidationLayerSupport() {
    uint32_t layerCount;
    if(vkEnumerateInstanceLayerProperties(&layerCount, nullptr) != VK_SUCCESS){
        throw std::runtime_error("VK : vkEnumerateInstanceLayerProperties error");
    }

    if(layerCount == 0){
        throw std::runtime_error("Failed to find any layer in your system.");
    }

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

VkBool32 debugCallback(VkDebugReportFlagsEXT flags,
                       VkDebugReportObjectTypeEXT objType,
                       uint64_t obj,
                       size_t location,
                       int32_t code,
                       const char* layerPrefix,
                       const char* msg,
                       void* userData) {
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if(queueFamily.queueCount>0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device,i,_surface,&presentSupport);
        if(presentSupport && queueFamily.queueCount>0){
            indices.presentFamily = i;
        }
        if(indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

void Application::run() {
    initWindow();
    initVulkan();
    mainLoop();
}

void Application::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);

    _window = glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"vulkan", nullptr, nullptr);
}

void Application::initVulkan() {

#ifdef ENABLE_VALIDATION_LAYERS
    if (!checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
#endif

    createInstance();

    // create surface
    if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

#ifdef ENABLE_VALIDATION_LAYERS
    //create debug report
    VkDebugReportCallbackCreateInfoEXT debugCreateInfoEXT = {};
    debugCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugCreateInfoEXT.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debugCreateInfoEXT.pfnCallback = (PFN_vkDebugReportCallbackEXT) debugCallback;

    if (CreateDebugReportCallbackEXT(_instance, &debugCreateInfoEXT, nullptr, &_callback) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug callback!\n");
    }
#endif

    createLogicalDevice();

}

void Application::createInstance() {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    instanceCreateInfo.enabledExtensionCount = extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

#ifdef ENABLE_VALIDATION_LAYERS
    instanceCreateInfo.enabledLayerCount = validationLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    if(vkCreateInstance(&instanceCreateInfo, nullptr,&_instance)!=VK_SUCCESS){
        throw std::runtime_error("failed to create instance!\n");
    }
}

void Application::createLogicalDevice() {
    //find PhysicalDevice
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance,&deviceCount, nullptr);
    if(deviceCount==0){
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance,&deviceCount,devices.data());

    QueueFamilyIndices indices = {};
    for (const auto& device : devices) {
        indices = findQueueFamilies(device);
        if(indices.isComplete()){
            physicalDevice = device;
            break;
        }
    }

    if(physicalDevice==VK_NULL_HANDLE){
        throw std::runtime_error("failed to find a suitable GPU!");
    }

#ifdef _DEBUG
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice,&physicalDeviceProperties);
    std::cerr << "physical device name : " << physicalDeviceProperties.deviceName << std::endl;
#endif

    //create logical device
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

    for (int queueFamily : uniqueQueueFamilies) {
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount = 0;

#ifdef ENABLE_VALIDATION_LAYERS
    deviceCreateInfo.enabledLayerCount = validationLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#else
    deviceCreateInfo.enabledLayerCount = 0;
#endif

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(_device, indices.graphicsFamily, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, indices.presentFamily, 0, &_presentQueue);
}

void Application::createSwapChain() {

}

void Application::mainLoop(){
    while(!glfwWindowShouldClose(_window)){
        glfwPollEvents();
    }
}