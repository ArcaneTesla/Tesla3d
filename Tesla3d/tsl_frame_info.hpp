#pragma once

#include "tsl_camera.hpp"
#include "tsl_scene_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace tsl {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        TslCamera& camera;
        VkDescriptorSet globalDescriptorSet;
        TslSceneObject::Map& sceneObjects;
    };
}  