#version 450

layout(set = 1, binding = 0) uniform samplerCube skyboxTexture;

layout(location = 0) in vec3 inPos;

layout(location = 0) out vec4 oColor;

void main() {
	oColor = texture(skyboxTexture, inPos);
}