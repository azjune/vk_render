//
// Created by ld016 on 16-8-11.
//

#include "v_window.h"
#include "v_renderer.h"
#include <assert.h>

v_window::v_window(v_renderer* renderer,uint32_t width, uint32_t height, const char *title) :
    _renderer(renderer),_width(width),_height(height),_title(title),_window_run(true)
{
    _initOSWindow();
    _initSurface();
    _initSwapchain();
    _initSwapchainImageViews();
}

v_window::~v_window() {
    _deInitSwapchainImageViews();
    _deInitSwapchain();
    _deInitSurface();
    _deInitOSWindow();
}

void v_window::close() {
    _window_run = false;
}

bool v_window::update() {
    _updateOSWindow();
    return _window_run;
}

void v_window::_deInitSurface() {
    vkDestroySurfaceKHR(_renderer->getInstance(),_surface, nullptr);
    _surface = VK_NULL_HANDLE;
}

void v_window::_initSurface() {
    if(_surface != VK_NULL_HANDLE){
        _deInitSurface();
    }

    _initOSSurface();

    auto gpu = _renderer->getPhysicalDevice();
    VkBool32 WSI_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(gpu,_renderer->getGraphicsFamilyIndex(),_surface,&WSI_support);
    assert(WSI_support && "WSI not support");

    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu,_surface,&capabilities);
    if(capabilities.currentExtent.width < UINT32_MAX){
        _width = capabilities.currentExtent.width;
        _height = capabilities.currentExtent.height;
    }
    {
        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu,_surface,&format_count, nullptr);
        assert(format_count>0 && "Surface formats missing.");

        std::vector<VkSurfaceFormatKHR> formats( format_count );
        vkGetPhysicalDeviceSurfaceFormatsKHR( gpu, _surface, &format_count, formats.data() );

        if(formats[0].format == VK_FORMAT_UNDEFINED){
            _surface_format.format = VK_FORMAT_R8G8B8A8_UNORM;
            _surface_format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        }else{
            _surface_format = formats[0];
        }
    }
    {
        if( _swapchain_image_count < capabilities.minImageCount + 1 ) _swapchain_image_count = capabilities.minImageCount + 1;
        if( capabilities.maxImageCount > 0 ) {
            if( _swapchain_image_count > capabilities.maxImageCount ) _swapchain_image_count = capabilities.maxImageCount;
        }
    }
}

void v_window::_initSwapchain() {
    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_renderer->getPhysicalDevice(),_surface,&capabilities);
    if(capabilities.currentExtent.width < UINT32_MAX){
        _width = capabilities.currentExtent.width;
        _height = capabilities.currentExtent.height;
    }
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    {
        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR( _renderer->getPhysicalDevice(), _surface, &present_mode_count, nullptr );
        std::vector<VkPresentModeKHR> present_mode_list( present_mode_count );
        vkGetPhysicalDeviceSurfacePresentModesKHR( _renderer->getPhysicalDevice(), _surface, &present_mode_count, present_mode_list.data() );
        for( auto m : present_mode_list ) {
            if( m == VK_PRESENT_MODE_MAILBOX_KHR ) present_mode = m;
        }
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    {
        createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface                  = _surface;
        createInfo.minImageCount            = _swapchain_image_count;
        createInfo.imageFormat              = _surface_format.format;
        createInfo.imageColorSpace          = _surface_format.colorSpace;
        createInfo.imageExtent              = {_width,_height};
        createInfo.imageArrayLayers         = 1;
        createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount    = 0;
        createInfo.pQueueFamilyIndices      = nullptr;
        createInfo.preTransform				= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createInfo.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode				= present_mode;
        createInfo.clipped					= VK_TRUE;
        createInfo.oldSwapchain				= _swapchain;
    }
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(_renderer->getDevice(),&createInfo, nullptr,&swapchain);
    assert(swapchain && "create swapchain faild");

    if(_swapchain != VK_NULL_HANDLE){
        vkDestroySwapchainKHR(_renderer->getDevice(),_swapchain, nullptr);
    }

    _swapchain = swapchain;
    vkGetSwapchainImagesKHR(_renderer->getDevice(),_swapchain,&_swapchain_image_count, nullptr);
    _swapchain_images.resize(_swapchain_image_count);
    vkGetSwapchainImagesKHR(_renderer->getDevice(),_swapchain,&_swapchain_image_count,_swapchain_images.data());
}

void v_window::_deInitSwapchain() {
    vkDestroySwapchainKHR(_renderer->getDevice(),_swapchain, nullptr);
    _swapchain = VK_NULL_HANDLE;
}

void v_window::_initSwapchainImageViews() {
    _deInitSwapchainImageViews();
    _swapchain_image_views.resize(_swapchain_image_count);
    for( uint32_t i=0; i < _swapchain_image_count; ++i ) {
        VkImageViewCreateInfo image_view_create_info {};
        image_view_create_info.sType				                = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image				                = _swapchain_images[ i ];
        image_view_create_info.viewType				                = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format				                = _surface_format.format;
        image_view_create_info.components.r			                = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g			                = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b			                = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a			                = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel		= 0;
        image_view_create_info.subresourceRange.levelCount			= 1;
        image_view_create_info.subresourceRange.baseArrayLayer		= 0;
        image_view_create_info.subresourceRange.layerCount			= 1;

        vkCreateImageView( _renderer->getDevice(), &image_view_create_info, nullptr, &_swapchain_image_views[ i ] );
    }
}

void v_window::_deInitSwapchainImageViews() {
    for( auto view : _swapchain_image_views ) {
        if(view != VK_NULL_HANDLE){
            vkDestroyImageView( _renderer->getDevice(), view, nullptr );
            view = VK_NULL_HANDLE;
        }
    }
}







