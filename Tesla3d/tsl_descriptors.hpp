#pragma once

#include "tsl_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace tsl {

    class TslDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(TslDevice &tslDevice) : tslDevice{ tslDevice } {}

            Builder &addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<TslDescriptorSetLayout> build() const;

        private:
            TslDevice& tslDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        TslDescriptorSetLayout(
            TslDevice &tslDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~TslDescriptorSetLayout();
        TslDescriptorSetLayout(const TslDescriptorSetLayout&) = delete;
        TslDescriptorSetLayout& operator=(const TslDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        TslDevice &tslDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class TslDescriptorWriter;
    };

    class TslDescriptorPool {
    public:
        class Builder {
        public:
            Builder(TslDevice& tslDevice) : tslDevice{ tslDevice } {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<TslDescriptorPool> build() const;

        private:
            TslDevice& tslDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        TslDescriptorPool(
            TslDevice& tslDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~TslDescriptorPool();
        TslDescriptorPool(const TslDescriptorPool&) = delete;
        TslDescriptorPool& operator=(const TslDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        TslDevice& tslDevice;
        VkDescriptorPool descriptorPool;

        friend class TslDescriptorWriter;
    };

    class TslDescriptorWriter {
    public:
        TslDescriptorWriter(TslDescriptorSetLayout& setLayout, TslDescriptorPool& pool);

        TslDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        TslDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        TslDescriptorSetLayout &setLayout;
        TslDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace tsl