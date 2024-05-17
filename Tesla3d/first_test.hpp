#pragma once

#include "tsl_window.hpp"
#include "tsl_pipeline.hpp"
#include "tsl_device.hpp"
#include "tsl_swap_chain.hpp"
#include "tsl_model.hpp"

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
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();


		TslWindow tslWindow{ WIDTH, HEIGHT, "First TEST app" };
        TslDevice tslDevice{ tslWindow };
        TslSwapChain tslSwapChain{tslDevice, tslWindow.getExtent()};
        std::unique_ptr<TslPipeline> tslPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<TslModel> tslModel;
	};
}