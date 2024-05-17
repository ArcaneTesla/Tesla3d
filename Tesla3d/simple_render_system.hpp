#pragma once

#include "tsl_pipeline.hpp"
#include "tsl_device.hpp"
#include "tsl_scene_object.hpp"

#include <memory>
#include <vector>
#include <array>

namespace tsl {
    class SimpleRenderSystem {

    public:

        SimpleRenderSystem(TslDevice& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
        void renderObjects(VkCommandBuffer commandBuffer, std::vector<TslSceneObject> &sceneObjects);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        TslDevice& tslDevice;

        std::unique_ptr<TslPipeline> tslPipeline;
        VkPipelineLayout pipelineLayout;
    };
}