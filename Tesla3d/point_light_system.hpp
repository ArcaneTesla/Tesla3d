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
    class PointLightSystem {

    public:

        PointLightSystem(TslDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void update(FrameInfo& frameInfo, GlobalUbo& ubo);
        void render(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        TslDevice& tslDevice;

        std::unique_ptr<TslPipeline> tslPipeline;
        VkPipelineLayout pipelineLayout;
    };
}