#version 450

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTexCoord0;
layout(location = 3) in vec2 iTexCoord1;
layout(location = 4) in vec4 iVertexColour;

layout(set = 0, binding = 0) uniform SceneUBO {
	mat4 projection;
	mat4 view;
	mat4 model;
	vec4 position;
} sceneUbo;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord0;
layout(location = 3) out vec2 outTexCoord1;
layout(location = 4) out vec4 outVertexColour;

void main() {
	outPosition = iPosition;
	outNormal = iNormal;
	outTexCoord0 = iTexCoord0;
	outTexCoord1 = iTexCoord1;
	outVertexColour = iVertexColour;

	gl_Position = sceneUbo.projection * sceneUbo.view * sceneUbo.model * vec4(iPosition, 1.0f);
}