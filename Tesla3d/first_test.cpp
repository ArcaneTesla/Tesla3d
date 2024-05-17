#include "first_test.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace tsl {

    struct SimplerPushConstantData
    {
        glm::mat2 transform{ 1.f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp() {
        loadObjects();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(tslDevice.device(), pipelineLayout, nullptr);
    }

	void FirstApp::run() {

		while (!tslWindow.shouldClose()) {
			glfwPollEvents();
            drawFrame();
		}
        vkDeviceWaitIdle(tslDevice.device());
	}

    void FirstApp::loadObjects() {
        std::vector<TslModel::Vertex> vertices{
            {{0.0f,-0.5f}, {1.0f,0.0f,0.0f}},
            {{0.5f, 0.5f}, {0.0f,1.0f,0.0f}},
            {{-0.5f,0.5f}, {0.0f,0.0f,1.0f}},
        };

        auto tslModel = std::make_shared<TslModel>(tslDevice, vertices);

        auto triangle = TslObject::createObject();
            triangle.model = tslModel;
            triangle.color = { .1f, .8f, .1f };
            triangle.transform2d.translation.x = .2f;
            triangle.transform2d.scale = { 2.f, .5f };
            triangle.transform2d.rotation = .25f * glm::two_pi<float>();

            sceneObjects.push_back(std::move(triangle));

    }

    void FirstApp::createPipelineLayout() {

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
    void FirstApp::recreateSwapChain() {
        auto extent = tslWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = tslWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(tslDevice.device());
        //tslSwapChain = nullptr;
        if (tslSwapChain == nullptr) {
            tslSwapChain = std::make_unique<TslSwapChain>(tslDevice, extent);
        }
        else{
            tslSwapChain = std::make_unique<TslSwapChain>(tslDevice, extent, std::move(tslSwapChain));
            if (tslSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }
    void FirstApp::createPipeline() {
        assert(tslSwapChain != nullptr && "Ошибка:Нельзя создать конвеер до цепочки");
        assert(tslSwapChain != nullptr && "Ошибка:Нельзя создать конвеер до pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        TslPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = tslSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        tslPipeline = std::make_unique<TslPipeline>(tslDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void FirstApp::freeCommandBuffers() {
        vkFreeCommandBuffers(tslDevice.device(), tslDevice.getCommandPool(),static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::createCommandBuffers(){
        commandBuffers.resize(tslSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = tslDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(tslDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось выделить коммандный буфер!");
        }
    }

    void FirstApp::recordCommandBuffer(int imageIndex) {

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось начать запись коммандного буфера!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = tslSwapChain->getRenderPass();
        renderPassInfo.framebuffer = tslSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0,0 };
        renderPassInfo.renderArea.extent = tslSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(tslSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(tslSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, tslSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        renderObjects(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось записать коммандный буфер!");
        }
    }

    void FirstApp::renderObjects(VkCommandBuffer commandBuffer) {
        tslPipeline->bind(commandBuffer);

        for (auto& obj : sceneObjects) {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.0001f, glm::two_pi<float>());

            SimplerPushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

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

    void FirstApp::drawFrame(){
        uint32_t imageIndex;
        auto result = tslSwapChain->acquireNextImage(&imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Ошибка:Не удалось получить образ цепи изображения!");
        }
        recordCommandBuffer(imageIndex);
        result = tslSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || tslWindow.wasWindowResized()) {
            tslWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось вывести образ цепи изображения!");
        }
    }
}