#pragma once

#include "tsl_window.hpp"
#include "tsl_pipeline.hpp"
#include "tsl_device.hpp"
#include "tsl_swap_chain.hpp"
#include "tsl_scene_object.hpp"

#include <memory>
#include <vector>
#include <array>
#include <cassert>

namespace tsl {
    class TslRenderer {

    public:

        TslRenderer(TslWindow &window, TslDevice &device);
        ~TslRenderer();

        TslRenderer(const TslRenderer&) = delete;
        TslRenderer& operator=(const TslRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return tslSwapChain->getRenderPass(); }

        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get get frame when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:

        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();


        TslWindow &tslWindow;
        TslDevice &tslDevice;
        std::unique_ptr <TslSwapChain> tslSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted = false;
    };
}