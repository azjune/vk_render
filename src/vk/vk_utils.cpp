//
// Created by ld016 on 16-8-9.
//

#include "vk_utils.h"

using namespace vk_utils;

#ifdef VK_USE_PLATFORM_XCB_KHR
void vk_utils::createSurface(VkInstance instance,VkSurfaceKHR* pSurface,xcb_connection_t* connection,xcb_window_t window){
    VkXcbSurfaceCreateInfoKHR createInfoKHR;
    createInfoKHR.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfoKHR.connection = connection;
    createInfoKHR.window = window;
    auto result = vkCreateXcbSurfaceKHR(instance,&createInfoKHR, nullptr,pSurface);
    checkVulkanResult(result,"can not create surface");
}
#elif VK_USE_PLATFORM_WIN32_KHR
void vk_utils::createSurface(VkInstance instance,VkSurfaceKHR* pSurface,HINSTANCE hinstance,HWND hwnd){
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hinstance = hInstance;
        surfaceCreateInfo.hwnd = windowHandle;
        vkCreateWin32SurfaceKHR( instance, &surfaceCreateInfo, NULL, pSurface );
    }
#endif

vk_window* vk_utils::createWindow(unsigned width, unsigned height, const char* title){
    vk_window* window = new vk_window();
    window->width = width;
    window->height = height;
    window->title = title;

    int screenNumber = 0;
    window->connection = xcb_connect(nullptr,&screenNumber);
    const xcb_setup_t* setup = xcb_get_setup(window->connection);
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = it.data;

    window->window = xcb_generate_id(window->connection);

    int16_t x=0,y=0;
    xcb_create_window(window->connection,
                      XCB_COPY_FROM_PARENT,
                      window->window,
                      screen->root,
                      x,y,
                      window->width,window->height,
                      1,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      0,
                      nullptr);
    xcb_map_window(window->connection,window->window);
    xcb_flush(window->connection);
    return window;
}

void vk_utils::getPhysicalDevice(VkPhysicalDevice& physicalDevice, VkInstance& instance,VkSurfaceKHR& surface,uint32_t queueFamilyIndex){
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount, physicalDevices.data());
    physicalDevice = physicalDevices[0];
    VkBool32 isSuppored = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,queueFamilyIndex,surface,&isSuppored);
}