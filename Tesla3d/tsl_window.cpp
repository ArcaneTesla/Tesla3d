#include "tsl_window.hpp"
#include <stdexcept>
namespace tsl {
    TslWindow::TslWindow(int w, int h, std::string name) : width{ w }, height{ w }, windowName{ name } {
        initWindow();
    }
    TslWindow::~TslWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void TslWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

    }

    void TslWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Ошибка:Не удалось создать поверхность окна!");
        }
    };
}