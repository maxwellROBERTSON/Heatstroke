#version 450

layout(location = 0) in vec3 iPosition;

layout(set = 0, binding = 0) uniform SceneUBO {
	mat4 projection;
	mat4 view;
	vec4 position;
} sceneUbo;

layout(set = 1, binding = 0) uniform Transforms {
	mat4 transform[100];
} transforms;

layout(location = 0) out vec2 uv;

const vec2 uvCoords[6] = vec2[6](
	vec2(0.0f, 0.0f), // Top left
	vec2(1.0f, 0.0f), // Top right
	vec2(1.0f, 1.0f), // Bottom right
	vec2(0.0f, 0.0f), // Top left
	vec2(1.0f, 1.0f), // Bottom right
	vec2(0.0f, 1.0f)  // Bottom left
);

void main() {
	uv = uvCoords[gl_VertexIndex];

	gl_Position = sceneUbo.projection * sceneUbo.view * transforms.transform[gl_InstanceIndex] * vec4(iPosition, 1.0f);
}