#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vk/v_renderer.h"

void render(v_renderer& renderer){
//    uint32_t nextImageIdx;
//    vkAcquireNextImageKHR( context.device, context.swapChain, UINT64_MAX,
//                           VK_NULL_HANDLE, VK_NULL_HANDLE, &nextImageIdx );
//
//    VkPresentInfoKHR presentInfo = {};
//    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//    presentInfo.pNext = NULL;
//    presentInfo.waitSemaphoreCount = 0;
//    presentInfo.pWaitSemaphores = NULL;
//    presentInfo.swapchainCount = 1;
//    presentInfo.pSwapchains = &context.swapChain;
//    presentInfo.pImageIndices = &nextImageIdx;
//    presentInfo.pResults = NULL;
//    vkQueuePresentKHR( context.graphicsQueue, &presentInfo );
}

int main() {
    v_renderer renderer;
    renderer.openWindow(800,600,"title");


    while (renderer.run()){

    }

    return 0;
}