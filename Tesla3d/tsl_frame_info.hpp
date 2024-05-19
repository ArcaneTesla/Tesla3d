#pragma once

#include "tsl_camera.hpp"
#include "tsl_scene_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace tsl {
    
    #define MAX_LIGHT 10

    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};
    };

    struct GlobalUbo {
        glm::mat4 projection{ 1.f };
        glm::mat4 view{ 1.f };
        glm::mat4 inverseView{ 1.f };
        glm::vec4 ambientLightColor{ 1.f,1.f,1.f, .02f };
        PointLight pointLights[MAX_LIGHT];
        int numLights;

    };
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        TslCamera& camera;
        VkDescriptorSet globalDescriptorSet;
        TslSceneObject::Map& sceneObjects;
    };
}  