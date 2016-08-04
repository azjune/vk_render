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

bool Application::isDeviceSuitable(VkPhysicalDevice device,VkSurfaceKHR surface) {
    vk_helper::QueueFamilyIndices indices = vk_helper::findQueueFamilies(device,surface);

    bool extensionsSupported = vk_helper::getDeviceExtension(device,deviceExtensions).empty();

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        vk_helper::SwapChainSupportDetails swapChainSupport = vk_helper::querySwapChainSupport(device,_surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported;// && swapChainAdequate;
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
    if (!vk_helper::checkValidationLayerSupport(validationLayers)) {
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

    if (vk_helper::createDebugReportCallbackEXT(_instance, &debugCreateInfoEXT, nullptr, &_callback) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug callback!\n");
    }
#endif

    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createGraphicsPipeline();
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

    auto extensions = vk_helper::getRequiredExtensions();
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


    for (const auto& device : devices) {
        if(isDeviceSuitable(device,_surface)){
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
    vk_helper::QueueFamilyIndices indices = vk_helper::findQueueFamilies(physicalDevice,_surface);
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

    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

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
    vk_helper::SwapChainSupportDetails swapChainSupport = vk_helper::querySwapChainSupport(physicalDevice,_surface);

    VkSurfaceFormatKHR surfaceFormat = vk_helper::chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = vk_helper::chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = vk_helper::chooseSwapExtent(swapChainSupport.capabilities,WINDOW_WIDTH,WINDOW_HEIGHT);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    vk_helper::QueueFamilyIndices indices = vk_helper::findQueueFamilies(physicalDevice,_surface);
    uint32_t queueFamilyIndices[] = {(uint32_t) indices.graphicsFamily, (uint32_t) indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;
}

void Application::createImageViews() {
    _swapChainImageViews.resize(_swapChainImages.size(), VDeleter<VkImageView>{_device, vkDestroyImageView});

    for (uint32_t i = 0; i < _swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = _swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void Application::createGraphicsPipeline() {
    VDeleter<VkShaderModule> vertShaderModule{_device, vkDestroyShaderModule};
    VDeleter<VkShaderModule> fragShaderModule{_device, vkDestroyShaderModule};
    vk_helper::createShaderModule("/home/ld016/web-app/vk_render/src/shader/vert.spv", &vertShaderModule,_device);
    vk_helper::createShaderModule("/home/ld016/web-app/vk_render/src/shader/frag.spv", &fragShaderModule,_device);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
}

void Application::mainLoop(){
    while(!glfwWindowShouldClose(_window)){
        glfwPollEvents();
    }
}