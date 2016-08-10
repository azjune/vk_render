//
// Created by ld016 on 16-8-9.
//

#include "vk_utils.h"

using namespace vk_utils;

#ifdef VK_USE_PLATFORM_XCB_KHR
VkSurfaceKHR vk_utils::createSurface(VkInstance instance,xcb_connection_t* connection,xcb_window_t window){
    VkSurfaceKHR surface;

    VkXcbSurfaceCreateInfoKHR createInfoKHR;
    createInfoKHR.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfoKHR.connection = connection;
    createInfoKHR.window = window;
    auto result = vkCreateXcbSurfaceKHR(instance,&createInfoKHR, nullptr,&surface);
    checkVulkanResult(result,"can not create surface");

    return surface;
}
#elif VK_USE_PLATFORM_WIN32_KHR
VkSurfaceKHR vk_utils::createSurface(VkInstance instance,HINSTANCE hinstance,HWND hwnd){
    VkSurfaceKHR surface;

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = hInstance;
    surfaceCreateInfo.hwnd = windowHandle;
    vkCreateWin32SurfaceKHR( instance, &surfaceCreateInfo, NULL, pSurface );

    return surface;
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

VkInstance vk_utils::createInstance(const char* appName, const char* engineName, bool enabledValidationLayer){
    VkApplicationInfo applicationInfo = {};
    {
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(0,1,0);
        applicationInfo.pApplicationName = appName;
        applicationInfo.pEngineName = engineName;
    }

    std::vector<const char*> extensionNames;
    uint32_t instanceExtensionPropertiesCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr,&instanceExtensionPropertiesCount, nullptr);
    std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionPropertiesCount);
    vkEnumerateInstanceExtensionProperties(nullptr,&instanceExtensionPropertiesCount, instanceExtensionProperties.data());
    for(auto& ex : instanceExtensionProperties){
        extensionNames.push_back(ex.extensionName);
    }

    VkInstanceCreateInfo instanceCreateInfo = {};
    {
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        instanceCreateInfo.enabledExtensionCount = extensionNames.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
        if(enabledValidationLayer){
            instanceCreateInfo.enabledLayerCount = validationLayerNames.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayerNames.data();
        }
    }

    VkInstance instance;
    auto result = vkCreateInstance(&instanceCreateInfo, nullptr,&instance);
    checkVulkanResult(result,"can not create instance");
    return instance;
}

VkPhysicalDevice vk_utils::getPhysicalDevice(uint32_t& queueFamilyIndex, VkInstance instance,VkSurfaceKHR surface){
    VkPhysicalDevice gpu;

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount, physicalDevices.data());

    for(auto& device : physicalDevices){
        uint32_t index = findQueueFamilyIndex(device);
        if(index!=-1){
            VkBool32 isSuppored = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device,index,surface,&isSuppored);
            if(isSuppored){
                gpu = device;
                queueFamilyIndex = index;
                break;
            }
        }
    }

    return gpu;
}

uint32_t vk_utils::findQueueFamilyIndex(VkPhysicalDevice& physicalDevice){
    uint32_t familyIndex = -1;

    uint32_t deviceQueueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&deviceQueueFamilyPropertiesCount, nullptr);
    std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&deviceQueueFamilyPropertiesCount, deviceQueueFamilyProperties.data());
    for(uint32_t i = 0;i<deviceQueueFamilyProperties.size();i++){
        if(deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
            familyIndex = i;
        }
    }
    return familyIndex;
}

VkSwapchainKHR vk_utils::createSwapChain(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface,VkDevice device) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,surface,&surfaceCapabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, surface, &formatCount, NULL );
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, surface, &formatCount, surfaceFormats.data() );
    //swap chain
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    {
        swapchainCreateInfo.sType               = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface             = surface;
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
    auto result = vkCreateSwapchainKHR(device,&swapchainCreateInfo, nullptr,&swapchain);
    checkVulkanResult(result,"can not create swap chain");
    return swapchain;
}

VkDevice vk_utils::createDevice(VkPhysicalDevice physicalDevice,uint32_t familyIndex,bool enabledValidationLayer) {
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice,&features);

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
    {
        float priorities = 1.0f;
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.pQueuePriorities = &priorities;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.queueFamilyIndex = familyIndex;
    }

    std::vector<const char*> extensionNames = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo = {};
    {
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pEnabledFeatures           = &features;
        deviceCreateInfo.queueCreateInfoCount       = 1;
        deviceCreateInfo.pQueueCreateInfos          = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledLayerCount          = validationLayerNames.size();
        deviceCreateInfo.ppEnabledLayerNames        = validationLayerNames.data();
        deviceCreateInfo.enabledExtensionCount      = extensionNames.size();
        deviceCreateInfo.ppEnabledExtensionNames    = extensionNames.data();
    }

    VkDevice device;
    auto result = vkCreateDevice(physicalDevice,&deviceCreateInfo, nullptr,&device);
    checkVulkanResult(result,"can not create device");
    return device;
}

std::vector<VkImage> vk_utils::getSwapchainImages(VkDevice device,VkSwapchainKHR swapchain){
    std::vector<VkImage> images;
    {
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(device,swapchain,&imageCount, nullptr);
        images.resize(imageCount);
        vkGetSwapchainImagesKHR(device,swapchain,&imageCount,images.data());
    }

    return images;
}


std::vector<VkImageView> vk_utils::createImageViews(VkDevice device,std::vector<VkImage>& images){


    VkImageViewCreateInfo createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.components = {   VK_COMPONENT_SWIZZLE_R,
                                    VK_COMPONENT_SWIZZLE_G,
                                    VK_COMPONENT_SWIZZLE_B,
                                    VK_COMPONENT_SWIZZLE_A};

        createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        createInfo.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT,
                0,
                1,
                0,
                1
        };
    }

    std::vector<VkImageView> imageViews(images.size());
    for(size_t i = 0;i<images.size();i++){
        createInfo.image = images[i];
        auto result = vkCreateImageView(device,&createInfo, nullptr,&imageViews[i]);
        checkVulkanResult( result, "Could not create ImageView." );
    }
    return imageViews;
}

