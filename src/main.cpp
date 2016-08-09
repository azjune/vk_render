#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vk/vk_context.h"
#include "vk/vk_utils.h"

int main() {
    auto window = vk_utils::createWindow(800,600,"test");

    vk_context c;

    VkSurfaceKHR surfaceKHR;
    vk_utils::createSurface(c.instance,&surfaceKHR,window->connection,window->window);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    {
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = c.familyIndex;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }

    VkCommandPool pool = VK_NULL_HANDLE;
    vkCreateCommandPool(c.device,&commandPoolCreateInfo, nullptr,&pool);


    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    {
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandBufferCount = 1;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = pool;
    }
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(c.device,&commandBufferAllocateInfo,&commandBuffer);


    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(c.physicalDevice,surfaceKHR,&surfaceCapabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR( c.physicalDevice, surfaceKHR, &formatCount, NULL );
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR( c.physicalDevice, surfaceKHR, &formatCount, surfaceFormats.data() );
    //swap chain
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    {
        swapchainCreateInfo.sType               = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface             = surfaceKHR;
        swapchainCreateInfo.minImageCount       = surfaceCapabilities.minImageCount;
        swapchainCreateInfo.imageFormat         = surfaceFormats[0].format;
        swapchainCreateInfo.imageColorSpace     = surfaceFormats[0].colorSpace;
        swapchainCreateInfo.imageExtent         = surfaceCapabilities.currentExtent;
        swapchainCreateInfo.imageArrayLayers    = 1;
        swapchainCreateInfo.imageUsage          = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode    = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.preTransform        = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha      = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode         = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.clipped             = true;
    }

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(c.device,&swapchainCreateInfo, nullptr,&swapchain);


    while (true){

    }

    vkDestroyCommandPool(c.device,pool, nullptr);
    return 0;
}