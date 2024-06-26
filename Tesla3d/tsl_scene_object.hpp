#pragma once

#include "tsl_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace tsl {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f , 1.f};
        glm::vec3 rotation{};
        
        glm::mat4 mat4();

        glm::mat3 normalMatrix();

    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };

    class TslSceneObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, TslSceneObject>;

        static TslSceneObject createObject() {
            static id_t currentId = 0;
            return TslSceneObject{ currentId++ };
        }

        static TslSceneObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

        TslSceneObject(const TslSceneObject&) = delete;
        TslSceneObject& operator=(const TslSceneObject&) = delete;
        TslSceneObject(TslSceneObject&&) = default;
        TslSceneObject& operator=(TslSceneObject&&) = default;

        id_t getId() { return id; }

        glm::vec3 color{};
        TransformComponent transform{};

        std::shared_ptr<TslModel> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        TslSceneObject(id_t objId) : id{ objId } {}

        id_t id;
    };
} 