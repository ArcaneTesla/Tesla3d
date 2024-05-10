#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace live {

	class LiveWindow {
	public:
		LiveWindow(int w, int h, std::string name);
		~LiveWindow();

		LiveWindow(const LiveWindow&) = delete;
		LiveWindow& operator=(const LiveWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

	private:

		void initWindow();

		const int width;
		const int height;

		std::string windowName;

		GLFWwindow *window;

	};
}