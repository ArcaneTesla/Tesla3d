#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace tsl {

    struct SimplerPushConstantData
    {
        glm::mat4 transform{ 1.f };
        glm::mat4 normalMatrix{ 1.f };
    };

    SimpleRenderSystem::SimpleRenderSystem(TslDevice& device, VkRenderPass renderPass) : tslDevice{device} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(tslDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplerPushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(tslDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось создать схему конвеера!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {

        assert(pipelineLayout != nullptr && "Ошибка:Нельзя создать конвеер до pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        TslPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        tslPipeline = std::make_unique<TslPipeline>(tslDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void SimpleRenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<TslSceneObject>& sceneObjects, const TslCamera& camera) {
        tslPipeline->bind(commandBuffer);

        auto projectionView = camera.getProjection() * camera.getView();

        for (auto& obj : sceneObjects) {
            SimplerPushConstantData push{};
            auto modelMatrix = obj.transform.mat4();
            push.transform = projectionView * modelMatrix;
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplerPushConstantData),
                &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}