#pragma once

#include "tsl_window.hpp"
#include "tsl_pipeline.hpp"
#include "tsl_device.hpp"
#include "tsl_swap_chain.hpp"
#include "tsl_object.hpp"

#include <memory>
#include <vector>
#include <array>

namespace tsl {
	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const TslWindow&) = delete;
        FirstApp& operator=(const TslWindow&) = delete;

		void run();

	private:
        void loadObjects();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void renderObjects(VkCommandBuffer commandBuffer);


		TslWindow tslWindow{ WIDTH, HEIGHT, "Tesla3D" };
        TslDevice tslDevice{ tslWindow };
        std::unique_ptr <TslSwapChain> tslSwapChain;
        std::unique_ptr<TslPipeline> tslPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<TslObject> sceneObjects;
	};
}