#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace tsl {

    struct SimplerPushConstantData
    {
        glm::mat4 modelMatrix{ 1.f };
        glm::mat4 normalMatrix{ 1.f };
    };

    SimpleRenderSystem::SimpleRenderSystem(TslDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : tslDevice{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(tslDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplerPushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(tslDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("������:�� ������� ������� ����� ��������!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {

        assert(pipelineLayout != nullptr && "������:������ ������� ������� �� pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        TslPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        tslPipeline = std::make_unique<TslPipeline>(tslDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void SimpleRenderSystem::renderSceneObjects(FrameInfo& frameInfo) {
        tslPipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        for (auto& kv : frameInfo.sceneObjects) {
            auto & obj = kv.second;
            if (obj.model == nullptr) continue;
            SimplerPushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplerPushConstantData),
                &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }
}