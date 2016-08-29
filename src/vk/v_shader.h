//
// Created by ld016 on 16-8-12.
//

#ifndef APP_V_SHADER_H
#define APP_V_SHADER_H

#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

class v_shader {
public:
    static VkShaderModule createShader(VkDevice device,const char* path){
        auto code = loadShaderFile(path);

        VkShaderModuleCreateInfo createInfo = {};
        {
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = (uint32_t*)code.data();
        }

        VkShaderModule shaderModule = VK_NULL_HANDLE;
        vkCreateShaderModule(device,&createInfo, nullptr,&shaderModule);
        return shaderModule;
    }

private:
    static std::vector<char> loadShaderFile(const char* path){
        std::ifstream file(path,std::ios::ate | std::ios::binary);

        assert(file.is_open() && "failed to open file!");
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
};


#endif //APP_V_SHADER_H
