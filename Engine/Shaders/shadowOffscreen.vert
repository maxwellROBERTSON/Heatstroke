#version 450

#define MAX_JOINTS 128

layout(location = 0) in vec3 iPosition;
layout(location = 1) in uvec4 iJoints;
layout(location = 2) in vec4 iWeights;

layout(set = 0, binding = 0) uniform Depth {
	mat4 depthMVP;
} depth;

layout(set = 1, binding = 0) uniform Node {
	mat4 jointMatrix[MAX_JOINTS];
	int isSkinned;
} node;

layout(push_constant) uniform ModelMatrix {
    mat4 model;
} modelMatrix;

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

	gl_Position = depth.depthMVP * transformationMatrix * vec4(iPosition, 1.0f);
}