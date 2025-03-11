#version 450

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec4 iTangent;
layout(location = 3) in vec2 iTexCoord0;
layout(location = 4) in vec2 iTexCoord1;
layout(location = 5) in vec4 iVertexColour;

layout(set = 0, binding = 0) uniform SceneUBO {
	mat4 projection;
	mat4 view;
	vec4 position;
} sceneUbo;

layout(set = 3, binding = 0) uniform modelMatrices {
	mat4 model;
} modelMatrix;

layout(location = 0) out vec3 v2fNormal;
layout(location = 1) out vec4 v2fTangent;
layout(location = 2) out vec2 v2fTexCoord0;
layout(location = 3) out vec2 v2fTexCoord1;
layout(location = 4) out vec4 v2fVertexColour;

void main() {
	v2fNormal = normalize(transpose(inverse(mat3(modelMatrix.model))) * iNormal);
	v2fTangent = iTangent;
	v2fTexCoord0 = iTexCoord0;
	v2fTexCoord1 = iTexCoord1;
	v2fVertexColour = iVertexColour;

	gl_Position = sceneUbo.projection * sceneUbo.view * modelMatrix.model * vec4(iPosition, 1.0f);
}