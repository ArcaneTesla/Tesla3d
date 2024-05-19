#pragma once

#include "tsl_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace tsl {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f , 1.f};
        glm::vec3 rotation{};
        
        glm::mat4 mat4();

        glm::mat3 normalMatrix();

    };

    class TslSceneObject {
    public:
        using id_t = unsigned int;

        static TslSceneObject createObject() {
            static id_t currentId = 0;
            return TslSceneObject{ currentId++ };
        }

        TslSceneObject(const TslSceneObject&) = delete;
        TslSceneObject& operator=(const TslSceneObject&) = delete;
        TslSceneObject(TslSceneObject&&) = default;
        TslSceneObject& operator=(TslSceneObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<TslModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        TslSceneObject(id_t objId) : id{ objId } {}

        id_t id;
    };
} 