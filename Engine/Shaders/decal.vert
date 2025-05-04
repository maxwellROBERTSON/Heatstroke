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

const vec2 uvCoords[4] = vec2[4](
	vec2(1.0f, 0.0f),
	vec2(0.0f, 0.0f),
	vec2(0.0f, 1.0f),
	vec2(1.0f, 1.0f) 
);

void main() {
	uv = uvCoords[gl_VertexIndex];

	gl_Position = sceneUbo.projection * sceneUbo.view * transforms.transform[gl_InstanceIndex] * vec4(iPosition, 1.0f);
}