#version 450

layout(location = 0) in vec3 iPosition;

layout(set = 0, binding = 0) uniform Depth {
	mat4 depthMVP;
} depth;

layout(set = 1, binding = 0) uniform ModelMatrices {
	mat4 model;
} modelMatrix;

void main() {
	gl_Position = depth.depthMVP * modelMatrix.model * vec4(iPosition, 1.0f);
}