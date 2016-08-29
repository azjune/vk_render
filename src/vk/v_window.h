//
// Created by ld016 on 16-8-11.
//

#ifndef APP_V_WINDOW_H
#define APP_V_WINDOW_H

#include "v_platform.hpp"
#include <vector>

class v_renderer;

class v_window {
public:
    v_window(v_renderer* renderer,uint32_t width,uint32_t height,const char* title);
    ~v_window();

    void close();
    bool update();

    inline const uint32_t getWidth(){
        return _width;
    }

    inline const uint32_t getHeight(){
        return _height;
    }

    inline const VkSurfaceFormatKHR getFormat(){
        return _surface_format;
    }

    inline const std::vector<VkImageView>& getImageViews(){
        return _swapchain_image_views;
    }

    inline const VkSwapchainKHR& getSwapchain(){
        return _swapchain;
    }

private:
    void _initOSWindow();
    void _deInitOSWindow();
    void _updateOSWindow();
    void _initOSSurface();

    void _initSurface();
    void _deInitSurface();
    void _initSwapchain();
    void _deInitSwapchain();

    void _initSwapchainImages();
    void _deInitSwapchainImages();

    v_renderer* _renderer;

    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

    std::vector<VkImage> _swapchain_images;
    std::vector<VkImageView> _swapchain_image_views;

    VkSurfaceFormatKHR _surface_format = {};
    VkSurfaceCapabilitiesKHR _surface_capabilities = {};
    uint32_t _swapchain_image_count = 2;

    uint32_t _width = 512;
    uint32_t _height = 512;
    const char* _title;

    bool _window_run = false;

#if VK_USE_PLATFORM_XCB_KHR
    xcb_connection_t* _xcb_connection = nullptr;
    xcb_window_t _xcb_window = 0;
    xcb_intern_atom_reply_t* _xcb_atom_window_reply	= nullptr;
#endif
};


#endif //APP_V_WINDOW_H
