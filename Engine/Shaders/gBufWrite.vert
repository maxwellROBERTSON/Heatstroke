#version 450

#define MAX_JOINTS 128

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec4 iTangent;
layout(location = 3) in vec2 iTexCoord0;
layout(location = 4) in vec2 iTexCoord1;
layout(location = 5) in vec4 iVertexColour;
layout(location = 6) in uvec4 iJoints;
layout(location = 7) in vec4 iWeights;

layout(set = 0, binding = 0) uniform SceneUBO {
	mat4 projection;
	mat4 view;
	vec4 position;
} sceneUbo;

layout(set = 1, binding = 0) uniform modelMatrices {
	mat4 model;
} modelMatrix;

layout(set = 2, binding = 0) uniform Node {
	mat4 jointMatrix[MAX_JOINTS];
	int isSkinned;
} node;

layout(location = 0) out vec3 v2fNormal;
layout(location = 1) out vec4 v2fTangent;
layout(location = 2) out vec2 v2fTexCoord0;
layout(location = 3) out vec2 v2fTexCoord1;
layout(location = 4) out vec4 v2fVertexColour;

void main() {
	mat4 transformationMatrix = modelMatrix.model;

	if (node.isSkinned == 1) {
		mat4 skinMatrix =
			iWeights.x * node.jointMatrix[int(iJoints.x)] +
			iWeights.y * node.jointMatrix[int(iJoints.y)] +
			iWeights.z * node.jointMatrix[int(iJoints.z)] +
			iWeights.w * node.jointMatrix[int(iJoints.w)];

		transformationMatrix = transformationMatrix * skinMatrix;
	}

	mat3 normalMatrix = transpose(inverse(mat3(transformationMatrix)));

	v2fNormal = normalize(normalMatrix * iNormal);
	v2fTangent = vec4(normalMatrix * iTangent.rgb, iTangent.w);
	v2fTexCoord0 = iTexCoord0;
	v2fTexCoord1 = iTexCoord1;
	v2fVertexColour = iVertexColour;

	gl_Position = sceneUbo.projection * sceneUbo.view * transformationMatrix * vec4(iPosition, 1.0f);
}