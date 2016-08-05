//
// Created by ld016 on 16-8-2.
//

#ifndef APP_APPLICATION_H
#define APP_APPLICATION_H

#include <iostream>

#include "vk/vk_helper.h"
#include "vk/vk_Context.h"

#include "tools/VDeleter.h"


class Application {
public:
    void run();
    static void onWindowResized(GLFWwindow* window, int width, int height);
    void recreateSwapChain();
private:
    void initWindow();
    void initVulkan();
    void mainLoop();

    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSemaphores();
    void drawFrame();

    bool isDeviceSuitable(VkPhysicalDevice device,VkSurfaceKHR surface);
private:
    GLFWwindow* _window;
    vk_Context _context;
    vk::SurfaceKHR _surface;
    VDeleter<VkDevice> _device{vkDestroyDevice};

    vk::PhysicalDevice physicalDevice;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;

    VDeleter<VkSwapchainKHR> _swapChain{_device, vkDestroySwapchainKHR};
    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;

    std::vector<VDeleter<VkImageView>> _swapChainImageViews;
    std::vector<VDeleter<VkFramebuffer>> _swapChainFramebuffers;
    VDeleter<VkRenderPass> _renderPass{_device, vkDestroyRenderPass};
    VDeleter<VkPipelineLayout> _pipelineLayout{_device, vkDestroyPipelineLayout};

    VDeleter<VkPipeline> _graphicsPipeline{_device, vkDestroyPipeline};
    VDeleter<VkCommandPool> _commandPool{_device, vkDestroyCommandPool};
    std::vector<VkCommandBuffer> _commandBuffers;

    VDeleter<VkSemaphore> _imageAvailableSemaphore{_device, vkDestroySemaphore};
    VDeleter<VkSemaphore> _renderFinishedSemaphore{_device, vkDestroySemaphore};
};

#endif //APP_APPLICATION_H
