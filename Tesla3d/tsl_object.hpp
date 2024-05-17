#pragma once

#include "tsl_model.hpp"

// std
#include <memory>

namespace tsl {

    struct Transform2dComponent {
        glm::vec2 translation{};  // (position offset)
        glm::vec2 scale{ 1.f, 1.f };
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{ {c, s}, {-s, c} };

            glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
            return rotMatrix * scaleMat;
        }
    };

    class TslObject {
    public:
        using id_t = unsigned int;

        static TslObject createObject() {
            static id_t currentId = 0;
            return TslObject{ currentId++ };
        }

        TslObject(const TslObject&) = delete;
        TslObject& operator=(const TslObject&) = delete;
        TslObject(TslObject&&) = default;
        TslObject& operator=(TslObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<TslModel> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d{};

    private:
        TslObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}  // namespace lve