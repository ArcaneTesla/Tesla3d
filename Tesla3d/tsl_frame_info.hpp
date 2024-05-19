#pragma once

#include "tsl_camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace tsl {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        TslCamera& camera;
    };
}  