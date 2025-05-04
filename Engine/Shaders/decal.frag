#version 450

layout(location = 0) in vec2 uv;

layout(set = 0, binding = 0) uniform SceneUBO {
	mat4 projection;
	mat4 view;
	vec4 position;
} sceneUbo;

layout(set = 2, binding = 0) uniform sampler2D decalTex;

layout(location = 0) out vec4 oColor;

void main() {
	oColor = texture(decalTex, uv);

	if (oColor.a < 0.25) {
		discard;
	}
}