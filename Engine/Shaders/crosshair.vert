#version 450

layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec4 iColor;

layout(set = 0, binding = 0) uniform OrthoUBO {
	mat4 projection;
} orthoUbo;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = iColor;

	gl_Position = orthoUbo.projection * vec4(iPosition, 0.0f, 1.0f);
}