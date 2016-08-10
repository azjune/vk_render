#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vk/vk_context.h"
#include "vk/vk_utils.h"

void destroyImageViews(VkDevice device,std::vector<VkImageView>& imageViews){
    for(size_t i = 0;i<imageViews.size();i++){
        vkDestroyImageView(device,imageViews[i], nullptr);
        imageViews[i] = VK_NULL_HANDLE;
    }
}

void render(vk_context& context){
    uint32_t nextImageIdx;
    vkAcquireNextImageKHR( context.device, context.swapChain, UINT64_MAX,
                           VK_NULL_HANDLE, VK_NULL_HANDLE, &nextImageIdx );

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pWaitSemaphores = NULL;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context.swapChain;
    presentInfo.pImageIndices = &nextImageIdx;
    presentInfo.pResults = NULL;
    vkQueuePresentKHR( context.graphicsQueue, &presentInfo );
}

int main() {
    auto window = vk_utils::createWindow(800,600,"test");

    vk_context context(window);


    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    {
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = context.familyIndex;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }

    VkCommandPool pool = VK_NULL_HANDLE;
    vkCreateCommandPool(context.device,&commandPoolCreateInfo, nullptr,&pool);


    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    {
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandBufferCount = 1;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = pool;
    }
    VkCommandBuffer setupCommandBuffer = VK_NULL_HANDLE;
    VkCommandBuffer drawCommandBuffer = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(context.device,&commandBufferAllocateInfo,&setupCommandBuffer);
    vkAllocateCommandBuffers(context.device, &commandBufferAllocateInfo, &drawCommandBuffer );

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence submitFence;
    vkCreateFence( context.device, &fenceCreateInfo, NULL, &submitFence );

    auto imageViews = vk_utils::createImageViews(context.device,context.images);
    for(size_t i = 0;i<imageViews.size();i++){
        // start recording on our setup command buffer:
        vkBeginCommandBuffer( setupCommandBuffer, &beginInfo );

        VkImageMemoryBarrier layoutTransitionBarrier = {};
        layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        layoutTransitionBarrier.srcAccessMask = 0;
        layoutTransitionBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        layoutTransitionBarrier.image = context.images[i];
        VkImageSubresourceRange resourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        layoutTransitionBarrier.subresourceRange = resourceRange;

        vkCmdPipelineBarrier(   setupCommandBuffer,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                0,
                                0, NULL,
                                0, NULL,
                                1, &layoutTransitionBarrier );

        vkEndCommandBuffer( setupCommandBuffer );

        // submitting code to the queue:
        VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = NULL;
        submitInfo.pWaitDstStageMask = waitStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &setupCommandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = NULL;
        vkQueueSubmit( context.graphicsQueue, 1, &submitInfo, submitFence );

        // waiting for it to finish:
        vkWaitForFences( context.device, 1, &submitFence, VK_TRUE, UINT64_MAX );
        vkResetFences( context.device, 1, &submitFence );

        vkResetCommandBuffer( setupCommandBuffer, 0 );
    }



    while (true){
        render(context);
    }

    vkDestroyCommandPool(context.device,pool, nullptr);

    destroyImageViews(context.device,imageViews);
    return 0;
}