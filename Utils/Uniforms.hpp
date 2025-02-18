#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace glsl {
    struct SceneUniform {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
        glm::vec4 position;
    };
}