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

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outTangent;
layout(location = 3) out vec2 outTexCoord0;
layout(location = 4) out vec2 outTexCoord1;
layout(location = 5) out vec4 outVertexColour;

void main() {
	outPosition = vec3(modelMatrix.model * vec4(iPosition, 1.0f));
	// We obviously transform the normals here since we may have transformed
	// the vertex positions and as such we need to also transform their normals,
	// however this may then invalidate the corresponding tangents that were
	// calculated on the CPU using the non-transformed normals, although objects
	// still look correct as far as I can tell so it may be fine.
	outNormal = normalize(transpose(inverse(mat3(modelMatrix.model))) * iNormal);
	outTangent = iTangent;
	outTexCoord0 = iTexCoord0;
	outTexCoord1 = iTexCoord1;
	outVertexColour = iVertexColour;

	gl_Position = sceneUbo.projection * sceneUbo.view * modelMatrix.model * vec4(iPosition, 1.0f);
}