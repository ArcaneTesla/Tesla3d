#pragma once

#include "tsl_device.hpp"
#include "tsl_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace tsl {
    class TslModel {
    public:

        struct Vertex{
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 normal;
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex& other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string& filepath);
        };

        TslModel(TslDevice &device, const TslModel::Builder &builder);
        ~TslModel();

        TslModel(const TslModel &) = delete;
        TslModel &operator=(const TslModel &) = delete;

        static std::unique_ptr<TslModel> createModelFromFile(TslDevice &device, const std::string &filepath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        TslDevice &tslDevice;

        std::unique_ptr<TslBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<TslBuffer> indexBuffer;
        uint32_t indexCount;
    };
}