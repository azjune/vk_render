//
// Created by ld016 on 16-8-5.
//

#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <vulkan/vulkan.hpp>
#include "vk_helper.h"

class vk_Context {
public:
    vk_Context();
    ~vk_Context();

    void creatInstance(const char* appName = "app", const char* engineName = "engine");

public:
    vk::Instance instance;
};


#endif //APP_CONTEXT_H
