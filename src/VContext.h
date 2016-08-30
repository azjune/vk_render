//
// Created by ld016 on 16-8-30.
//

#ifndef APP_VCONTEXT_H
#define APP_VCONTEXT_H


#include <vulkan/vulkan.hpp>

class VContext {
public:
    vk::Instance        instance;
    vk::PhysicalDevice  physicalDevice;
    vk::Device          device;
    uint32_t            graphics_family_index;

private:
    void _initInstance();
    void _initPhysicalDevice();
};


#endif //APP_VCONTEXT_H
