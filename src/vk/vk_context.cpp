//
// Created by ld016 on 16-8-8.
//

#include "vk_context.h"

vk_context::vk_context(vk_utils::vk_window* window):_window(window) {
    init();
}

vk_context::~vk_context() {
    destroy();
}

void vk_context::init() {
    instance = vk_utils::createInstance("app","engine",true);
    surface = vk_utils::createSurface(instance,_window->connection,_window->window);
    _debug.init(instance);
    physicalDevice = vk_utils::getPhysicalDevice(familyIndex,instance,surface);
    device = vk_utils::createDevice(physicalDevice,familyIndex,true);
    vkGetDeviceQueue(device,familyIndex,0,&graphicsQueue);
    swapChain = vk_utils::createSwapChain(physicalDevice,surface,device);

    images = vk_utils::getSwapchainImages(device,swapChain);
}

void vk_context::destroy() {
    vkDestroySwapchainKHR(device,swapChain, nullptr);
    swapChain = VK_NULL_HANDLE;
    vkDestroyDevice(device, nullptr);
    device = VK_NULL_HANDLE;
    _debug.deInit();
    vkDestroySurfaceKHR(instance,surface, nullptr);
    surface = VK_NULL_HANDLE;
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
}







