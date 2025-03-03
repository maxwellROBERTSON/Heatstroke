#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace glsl {
    struct SceneUniform {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec4 position;
    };

    struct ModelMatricesUniform {
        glm::mat4* model;
    };

    // Laying out the struct in groups of their
    // types seem to be critical to sending the data
    // to the GPU correctly. When the members are
    // ordered differently, the data seems to be incorrect
    // when read from the fragment shader
    struct alignas(16) MaterialInfoBuffer {
        glm::vec4 emissiveFactor;
        glm::vec4 baseColourFactor;
        int alphaMode;
        int emissiveTexSet;
        int baseColourTexSet;
        int normalTexSet;
        int metallicRoughnessTexSet;
        int occlusionTexSet;
        float alphaCutoff;
        float emissiveStrength;
        float occlusionStrength;
        float metallicFactor;
        float roughnessFactor;
    };
}