#pragma once

#include "tsl_window.hpp"
#include "tsl_device.hpp"
#include "tsl_scene_object.hpp"
#include "tsl_renderer.hpp"

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

        FirstApp(const FirstApp&) = delete;
        FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
        void loadObjects();

		TslWindow tslWindow{ WIDTH, HEIGHT, "Tesla3D" };
        TslDevice tslDevice{ tslWindow };
        TslRenderer tslRenderer{ tslWindow, tslDevice };

        std::vector<TslSceneObject> sceneObjects;
	};
}