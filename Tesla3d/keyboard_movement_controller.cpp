#include "keyboard_movement_controller.hpp"
#include <limits>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/compatibility.hpp>

namespace tsl {
    void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, TslSceneObject& sceneObject) {
        // Управление поворотами с помощью стрелок
        float rotationSpeed = 1.5f;
        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
            sceneObject.transform.rotation.y += rotationSpeed * dt;
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
            sceneObject.transform.rotation.y -= rotationSpeed * dt;
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
            sceneObject.transform.rotation.x += rotationSpeed * dt;
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
            sceneObject.transform.rotation.x -= rotationSpeed * dt;

        // Управление поворотами с помощью мыши, только если зажата средняя кнопка мыши
        static bool mouseButtonDown = false;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            static double lastX = xpos;
            static double lastY = ypos;

            if (!mouseButtonDown) {
                lastX = xpos;
                lastY = ypos;
                mouseButtonDown = true;
            }

            float sensitivity = 10.f; // Чувствительность мыши

            float xOffset = (xpos - lastX) * sensitivity;
            float yOffset = (lastY - ypos) * sensitivity; // Инвертируем ось Y

            // Обновляем углы поворота сцены
            sceneObject.transform.rotation.y += xOffset * dt;
            sceneObject.transform.rotation.x += yOffset * dt;

            // Убеждаемся, что угол вращения по оси x находится в диапазоне [-π/2, π/2]
            sceneObject.transform.rotation.x = glm::clamp(sceneObject.transform.rotation.x, -glm::half_pi<float>(), glm::half_pi<float>());

            lastX = xpos;
            lastY = ypos;
        }
        else {
            mouseButtonDown = false;
        }

        const float yaw = sceneObject.transform.rotation.y;
        const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
        const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
        const glm::vec3 upDir{ 0.f, 1.f, 0.f };

        glm::vec3 moveDir{ 0.f };
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            sceneObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
    }
}