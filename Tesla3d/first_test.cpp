#include "first_test.hpp"

namespace live {

	void FirstApp::run() {

		while (!liveWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}