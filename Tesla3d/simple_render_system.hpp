#pragma once

#include "tsl_camera.hpp"
#include "tsl_pipeline.hpp"
#include "tsl_device.hpp"
#include "tsl_scene_object.hpp"
#include "tsl_frame_info.hpp"

#include <memory>
#include <vector>
#include <array>

namespace tsl {
    class SimpleRenderSystem {

    public:

        SimpleRenderSystem(TslDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderSceneObjects(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        TslDevice& tslDevice;

        std::unique_ptr<TslPipeline> tslPipeline;
        VkPipelineLayout pipelineLayout;
    };
}