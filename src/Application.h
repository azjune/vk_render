//
// Created by ld016 on 16-8-2.
//

#ifndef APP_APPLICATION_H
#define APP_APPLICATION_H

#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VDeleter.h"

#ifdef _DEBUG
#define ENABLE_VALIDATION_LAYERS
#endif


VkResult CreateDebugReportCallbackEXT(VkInstance instance,
                                      const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugReportCallbackEXT* pCallback);
void DestroyDebugReportCallbackEXT(VkInstance instance,
                                   VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks* pAllocator);

struct QueueFamilyIndices {
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete() {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

class Application {
public:
    void run();


private:
    void initWindow();
    void initVulkan();
    void mainLoop();

    void createInstance();
    void createLogicalDevice();
    void createSwapChain();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
private:
    GLFWwindow* _window;
    VDeleter<VkInstance> _instance{vkDestroyInstance};
    VDeleter<VkDebugReportCallbackEXT> _callback{_instance, DestroyDebugReportCallbackEXT};
    VDeleter<VkSurfaceKHR> _surface{_instance, vkDestroySurfaceKHR};
    VDeleter<VkDevice> _device{vkDestroyDevice};

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;
};

#endif //APP_APPLICATION_H
