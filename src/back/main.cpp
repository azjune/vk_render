#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vk/v_renderer.h"
#include "vk/v_shader.h"

std::vector<VkFramebuffer> createFramebuffer(v_renderer& renderer,const VkRenderPass& pass){
    auto views = renderer.getWindow()->getImageViews();
    auto count = views.size();
    std::vector<VkFramebuffer> frameBuffers(count);
    for (auto i = 0;i<count;i++) {
        VkFramebufferCreateInfo createInfo = {};
        {
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &views[i];
            createInfo.renderPass = pass;
            createInfo.width = renderer.getWindow()->getWidth();
            createInfo.height = renderer.getWindow()->getHeight();
            createInfo.layers = 1;
        }

        vkCreateFramebuffer(renderer.getDevice(),&createInfo, nullptr,&frameBuffers[i]);
    }
    return frameBuffers;
}

VkPipelineShaderStageCreateInfo getShaderStageCreateInfo(const VkShaderStageFlagBits stage,const VkShaderModule module){
    VkPipelineShaderStageCreateInfo stageCreateInfo = {};
    stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageCreateInfo.stage = stage;
    /*
     shader 中的常量设置，可以优化性能。
     stageCreateInfo.pSpecializationInfo = nullptr;
     */
    stageCreateInfo.module = module;
    stageCreateInfo.pName = "main";
    return stageCreateInfo;
}

VkPipelineVertexInputStateCreateInfo getVertexInputState(){
    VkVertexInputBindingDescription binding = {};
    {
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        binding.binding = 0;
        binding.stride = 0;
    }

    VkVertexInputAttributeDescription attribute = {};
    {

    }

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    {
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;
    }

    return vertexInputStateCreateInfo;
}

VkRenderPass createRenderPass(v_renderer& renderer){

    VkAttachmentDescription attachmentDescription = {};
    {
        attachmentDescription.format = renderer.getWindow()->getFormat().format;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    VkAttachmentReference reference = {};
    {
        reference.attachment = 0;
        reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpassDescription = {};
    {
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &reference;
    }

    VkSubpassDependency dependency = {};
    {
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    VkRenderPassCreateInfo createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &attachmentDescription;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpassDescription;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;
    }
    VkRenderPass renderPass = VK_NULL_HANDLE;
    vkCreateRenderPass(renderer.getDevice(),&createInfo, nullptr,&renderPass);
    return renderPass;
}

VkPipelineLayout createPipelineLayout(v_renderer& renderer){
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo layoutCreateInfo = {};
    {
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pSetLayouts = nullptr;
        layoutCreateInfo.pushConstantRangeCount = 0;
        layoutCreateInfo.pPushConstantRanges = nullptr;
    }
    vkCreatePipelineLayout(renderer.getDevice(),&layoutCreateInfo, nullptr,&layout);
    return layout;
}

VkPipeline createGraphicsPipeline(v_renderer& renderer,const VkRenderPass& renderPass,VkPipelineLayout& layout,std::vector<VkPipelineShaderStageCreateInfo>& stages){
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = getVertexInputState();

    VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo = {};
    {
        assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    }

    VkViewport viewport = {};
    {
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = renderer.getWindow()->getWidth();
        viewport.height = renderer.getWindow()->getHeight();
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
    }

    VkRect2D scissor = {};
    {
        scissor.offset = {0,0};
        scissor.extent = {renderer.getWindow()->getWidth(),renderer.getWindow()->getHeight()};
    }

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    {
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;
    }

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    {
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;                       //如果设置为true 超出近平面和远平面的片段将不会被废弃。
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;                //如果为true，几何对象将不会通过光栅阶段。
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;               //顺时针方向为正面
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
        rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    }

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};               //多重采样，暂时不设置
    {
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateCreateInfo.minSampleShading = 1.0f;
        multisampleStateCreateInfo.pSampleMask = nullptr;
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    }

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    {
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable = VK_FALSE;
    }


    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    {
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
        colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[3] = 0.0f;
    }

    VkGraphicsPipelineCreateInfo createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = stages.size();
        createInfo.pStages = stages.data();
        createInfo.pVertexInputState = &vertexInputStateCreateInfo;
        createInfo.pInputAssemblyState = &assemblyStateCreateInfo;
        createInfo.pViewportState = &viewportStateCreateInfo;
        createInfo.pRasterizationState = &rasterizationStateCreateInfo;
        createInfo.pMultisampleState = &multisampleStateCreateInfo;
        createInfo.pDepthStencilState = nullptr;                                        //深度模板缓冲
        createInfo.pColorBlendState = &colorBlendStateCreateInfo;
        createInfo.pDynamicState = nullptr;                                             //动态状态，绘制时设值，这里设置的值将会覆盖之前设置的状态
        createInfo.layout = layout;
        createInfo.renderPass = renderPass;
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;
    }

    VkPipeline pipeline = VK_NULL_HANDLE;
    vkCreateGraphicsPipelines(renderer.getDevice(), nullptr,1,&createInfo, nullptr,&pipeline);
    return pipeline;
}

VkCommandPool createCommandPool(v_renderer& renderer){
    VkCommandPoolCreateInfo createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = renderer.getGraphicsFamilyIndex();
        createInfo.flags = 0;
    }
    VkCommandPool commandPool = VK_NULL_HANDLE;
    vkCreateCommandPool(renderer.getDevice(),&createInfo, nullptr,&commandPool);
    return commandPool;
}

std::vector<VkCommandBuffer> createCommandBuffers(v_renderer& renderer, const VkCommandPool& pool,const std::vector<VkFramebuffer>& framebuffers){

    VkCommandBufferAllocateInfo allocateInfo = {};
    {
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = pool;
        allocateInfo.commandBufferCount = framebuffers.size();
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    }
    std::vector<VkCommandBuffer> commandBuffers(framebuffers.size());
    vkAllocateCommandBuffers(renderer.getDevice(),&allocateInfo,commandBuffers.data());
    return commandBuffers;
}

void beginCommandBuffers(v_renderer& renderer,std::vector<VkCommandBuffer> commandBuffers,VkPipeline pipeline,VkRenderPass renderPass,std::vector<VkFramebuffer> frameBuffers){
    for (auto i = 0;i<commandBuffers.size();i++) {

        VkCommandBufferBeginInfo beginInfo = {};
        {
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            beginInfo.pInheritanceInfo = nullptr;
        }
        vkBeginCommandBuffer(commandBuffers[i],&beginInfo);

        VkClearValue clearColor = {0.0f,0.0f,0.0f,1.0f};
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        {
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = renderPass;
            renderPassBeginInfo.framebuffer = frameBuffers[i];
            renderPassBeginInfo.clearValueCount = 1;
            renderPassBeginInfo.pClearValues = &clearColor;
            renderPassBeginInfo.renderArea.offset = {0,0};
            renderPassBeginInfo.renderArea.extent = {renderer.getWindow()->getWidth(),renderer.getWindow()->getHeight()};
        }
        vkCmdBeginRenderPass(commandBuffers[i],&renderPassBeginInfo,VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i],VK_PIPELINE_BIND_POINT_GRAPHICS,pipeline);
        vkCmdDraw(commandBuffers[i],3,1,0,0);

        vkCmdEndRenderPass(commandBuffers[i]);
        vkEndCommandBuffer(commandBuffers[i]);
    }
}

VkSemaphore createSemaphores(v_renderer& renderer){

    VkSemaphoreCreateInfo createInfo = {};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    }
    VkSemaphore semaphore = VK_NULL_HANDLE;
    vkCreateSemaphore(renderer.getDevice(),&createInfo, nullptr,&semaphore);
    return semaphore;
}
void render(v_renderer& renderer,std::vector<VkCommandBuffer>& commandBuffers,VkSemaphore& imageAvailableSemaphore,VkSemaphore& renderFinishedSemaphore){
    uint32_t nextImageIdx;
    vkAcquireNextImageKHR( renderer.getDevice(),
                           renderer.getWindow()->getSwapchain(),
                           UINT64_MAX,
                           imageAvailableSemaphore,
                           VK_NULL_HANDLE,
                           &nextImageIdx );

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo = {};
    {
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[nextImageIdx];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphore;
    }

    vkQueueSubmit(renderer.getQueue(), 1, &submitInfo, VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo = {};
    {
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &renderer.getWindow()->getSwapchain();
        presentInfo.pImageIndices = &nextImageIdx;
        presentInfo.pResults = NULL;
    }
    vkQueuePresentKHR( renderer.getQueue(), &presentInfo );
}

int main(int argc,char* argv[]) {
    v_renderer renderer;
    renderer.openWindow(800,600,"title");

    VkRenderPass renderPass = createRenderPass(renderer);
    auto layout = createPipelineLayout(renderer);
    VkShaderModule vShaderModule = v_shader::createShader(renderer.getDevice(),"/home/ld016/web-app/vk_render/src/shader/vert.spv");
    VkPipelineShaderStageCreateInfo vShaderStage = getShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT,vShaderModule);
    VkShaderModule fShaderModule = v_shader::createShader(renderer.getDevice(),"/home/ld016/web-app/vk_render/src/shader/frag.spv");
    VkPipelineShaderStageCreateInfo fShaderStage = getShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT,fShaderModule);

    std::vector<VkPipelineShaderStageCreateInfo> stages = {
            vShaderStage,fShaderStage
    };
    auto pipeline = createGraphicsPipeline(renderer,renderPass,layout,stages);
    auto frameBuffers = createFramebuffer(renderer,renderPass);

    VkCommandPool commandPool = createCommandPool(renderer);
    auto commandBuffers = createCommandBuffers(renderer,commandPool,frameBuffers);

    beginCommandBuffers(renderer,commandBuffers,pipeline,renderPass,frameBuffers);

    renderer.getWindow()->onResize = [&](uint32_t width,uint32_t height){
        vkDestroyRenderPass(renderer.getDevice(),renderPass, nullptr);
        renderPass = createRenderPass(renderer);

        vkDestroyPipelineLayout(renderer.getDevice(),layout, nullptr);
        layout = createPipelineLayout(renderer);

        vkDestroyPipeline(renderer.getDevice(),pipeline, nullptr);
        pipeline = createGraphicsPipeline(renderer,renderPass,layout,stages);

        for (auto frameBuffer : frameBuffers) {
            vkDestroyFramebuffer(renderer.getDevice(),frameBuffer, nullptr);
        }
        frameBuffers = createFramebuffer(renderer,renderPass);

//        vkFreeCommandBuffers(renderer.getDevice(),commandPool,commandBuffers.size(),commandBuffers.data());
        commandBuffers = createCommandBuffers(renderer,commandPool,frameBuffers);
        beginCommandBuffers(renderer,commandBuffers,pipeline,renderPass,frameBuffers);
    };

    auto imageAvailableSemaphore = createSemaphores(renderer);
    auto renderFinishedSemaphore = createSemaphores(renderer);

    while (renderer.run()){
        render(renderer,commandBuffers,imageAvailableSemaphore,renderFinishedSemaphore);
        vkDeviceWaitIdle(renderer.getDevice());
    }

    vkFreeCommandBuffers(renderer.getDevice(),commandPool,commandBuffers.size(),commandBuffers.data());

    vkDestroyCommandPool(renderer.getDevice(),commandPool, nullptr);
    for (auto frameBuffer : frameBuffers) {
        vkDestroyFramebuffer(renderer.getDevice(),frameBuffer, nullptr);
    }
    vkDestroyPipeline(renderer.getDevice(),pipeline, nullptr);
    vkDestroyRenderPass(renderer.getDevice(),renderPass, nullptr);
    vkDestroyPipelineLayout(renderer.getDevice(),layout, nullptr);
    vkDestroyShaderModule(renderer.getDevice(),vShaderModule, nullptr);
    vkDestroyShaderModule(renderer.getDevice(),fShaderModule, nullptr);
    vkDestroySemaphore(renderer.getDevice(),imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(renderer.getDevice(),renderFinishedSemaphore, nullptr);

    return 0;
}