//
// Created by ld016 on 16-8-11.
//

#ifndef APP_V_RENDERER_H
#define APP_V_RENDERER_H

#include <vector>
#include "v_platform.hpp"

class v_window;

class v_renderer {
public:
    v_renderer();
    ~v_renderer();

    v_window* openWindow(uint32_t width,uint32_t height,const char* title);

    bool run();

    inline VkInstance getInstance(){
        return _instance;
    }

    inline VkPhysicalDevice getPhysicalDevice(){
        return _gpu;
    }

    inline uint32_t getGraphicsFamilyIndex(){
        return _graphics_family_index;
    }

    inline VkDevice getDevice(){
        return _device;
    }
private:
    void _initInstance();
    void _deInitInstance();

    void _initDevice();
    void _deInitDevice();

    void _initDebug();
    void _deInitDebug();

    VkInstance _instance = VK_NULL_HANDLE;
    VkPhysicalDevice _gpu = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkQueue _queue = VK_NULL_HANDLE;

    uint32_t _graphics_family_index = 0;

    v_window* _window = nullptr;

    const std::vector<const char*> _instance_layer = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    VkDebugReportCallbackEXT _debugReportCallback = VK_NULL_HANDLE;
};


#endif //APP_V_RENDERER_H
