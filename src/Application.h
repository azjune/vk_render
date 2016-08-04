//
// Created by ld016 on 16-8-2.
//

#ifndef APP_APPLICATION_H
#define APP_APPLICATION_H

#include <iostream>

#include "vk/vk_helper.h"

#include "tools/VDeleter.h"


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
    void createImageViews();
    void createGraphicsPipeline();

    bool isDeviceSuitable(VkPhysicalDevice device,VkSurfaceKHR surface);
private:
    GLFWwindow* _window;
    VDeleter<VkInstance> _instance{vkDestroyInstance};
    VDeleter<VkDebugReportCallbackEXT> _callback{_instance, vk_helper::destroyDebugReportCallbackEXT};
    VDeleter<VkSurfaceKHR> _surface{_instance, vkDestroySurfaceKHR};
    VDeleter<VkDevice> _device{vkDestroyDevice};

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;

    VDeleter<VkSwapchainKHR> _swapChain{_device, vkDestroySwapchainKHR};
    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;

    std::vector<VDeleter<VkImageView>> _swapChainImageViews;
};

#endif //APP_APPLICATION_H
