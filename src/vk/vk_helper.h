//
// Created by ld016 on 16-8-4.
//

#ifndef APP_VK_HELPER_H
#define APP_VK_HELPER_H

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <set>
#include <string.h>
#include <string>
#include <limits>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define ENABLE_VALIDATION_LAYERS 1

namespace vk_helper{
    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() {
            return graphicsFamily >= 0 && presentFamily >= 0;
        }
    };

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    std::set<std::string> getDeviceExtension(VkPhysicalDevice device,const std::vector<const char*>& deviceExtensions);


    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,VkSurfaceKHR surface);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, unsigned width, unsigned height);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,VkSurfaceKHR surface);

    std::vector<char> readFile(const std::string& filename);

    void createShaderModule(const std::string& path, VkShaderModule* shaderModule,VkDevice device);
}


#endif //APP_VK_HELPER_H
