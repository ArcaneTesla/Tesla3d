#include "first_test.hpp"
#include <stdexcept>

namespace tsl {

    FirstApp::FirstApp() {
        createPipelineLayout();
        createPipeline();
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

    void FirstApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(tslDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось создать схему конвеера!");
        }
    }
    void FirstApp::createPipeline() {
        auto pipelineConfig = TslPipeline::defaultPipelineConfigInfo(tslSwapChain.width(), tslSwapChain.height());
        pipelineConfig.renderPass = tslSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        tslPipeline = std::make_unique<TslPipeline>(tslDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);


    }
    void FirstApp::createCommandBuffers(){
        commandBuffers.resize(tslSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = tslDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(tslDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось выделить коммандный буфер!");
        }

        for (int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Ошибка:Не удалось начать запись коммандного буфера!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = tslSwapChain.getRenderPass();
            renderPassInfo.framebuffer = tslSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0,0 };
            renderPassInfo.renderArea.extent = tslSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            tslPipeline->bind(commandBuffers[i]);
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Ошибка:Не удалось записать коммандный буфер!");
            }
        }
    }
    void FirstApp::drawFrame(){
        uint32_t imageIndex;
        auto result = tslSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Ошибка:Не удалось получить образ цепи изображения!");
        }

        result = tslSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось вывести образ цепи изображения!");
        }
    }
}