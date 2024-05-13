#include "first_test.hpp"

namespace lve {

	void FirstApp::run() {

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}