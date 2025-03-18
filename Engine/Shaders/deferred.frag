#version 450

#define PI 3.14159265359

#define MAX_LIGHTS 5

layout(location = 0) in vec2 v2fTexCoord;

layout(set = 0, input_attachment_index = 0, binding = 0) uniform subpassInput inputNormals;
layout(set = 0, input_attachment_index = 1, binding = 1) uniform subpassInput inputAlbedo;
layout(set = 0, input_attachment_index = 2, binding = 2) uniform subpassInput inputEmissive;
layout(set = 0, input_attachment_index = 3, binding = 3) uniform subpassInput inputDepth;

layout(set = 1, binding = 0) uniform SceneUBO {
	mat4 projection;
	mat4 view;
	vec4 position;
} sceneUbo;

struct Light {
	vec4 pos;
	vec4 color;
};

layout(set = 2, binding = 0) uniform Lights {
	Light light[MAX_LIGHTS];
} lights;

layout(location = 0) out vec4 oColor;

vec3 posFromDepth(float depth) {
	vec4 clipSpace = vec4(v2fTexCoord * 2.0f - 1.0f, depth, 1.0f);
	vec4 viewSpace = inverse(sceneUbo.view) * inverse(sceneUbo.projection) * clipSpace;
	vec3 worldSpace = viewSpace.xyz / viewSpace.w;

	return worldSpace;
}

float DistributionFunction(vec3 normal, vec3 halfwayVector, float roughness) {
    // Normal distribution function
    float nDotH = max(dot(normal, halfwayVector), 0.0001f);
    float nDotH2 = nDotH * nDotH;
    float nDotH4 = nDotH2 * nDotH2;
    float roughness2 = roughness * roughness;

    float ndf_numerator = exp((nDotH2 - 1) / (roughness2 * nDotH2));
    float ndf_denom = PI * roughness2 * nDotH4;

    float ndf = ndf_numerator / (0.001f + ndf_denom); // Add an epsilon to denom to prevent / by 0
    return ndf;
}

vec3 Fresnel(float metalness, vec3 halfwayVector, vec3 viewDir) {
    // Fresnel
    // Specular base reflectivity
    vec3 f0 = (1 - metalness) * vec3(0.04) + (metalness * subpassLoad(inputAlbedo).rgb);
    vec3 fresnel = f0 + (1 - f0) * pow((1 - dot(halfwayVector, viewDir)), 5.0);
    return fresnel;
}

float GeometryFunction(vec3 normal, vec3 halfwayVector, vec3 viewDir, vec3 lightDir) {
    // Geometry function
    float termLeft = 2 * (max(0, dot(normal, halfwayVector)) * max(0, dot(normal, viewDir)) / dot(viewDir, halfwayVector));
    float termRight = 2 * (max(0, dot(normal, halfwayVector)) * max(0, dot(normal, lightDir)) / dot(viewDir, halfwayVector));

    float geometry = min(1, min(termLeft, termRight));
    return geometry;    
}

vec3 brdf(vec3 lightDir, vec3 viewDir, vec3 normal) {
	vec3 halfwayVector = normalize(viewDir + lightDir);

	float metalness = subpassLoad(inputNormals).a;
	float roughness = subpassLoad(inputAlbedo).a;

	float ndf = DistributionFunction(normal, halfwayVector, roughness);
	vec3 fresnel = Fresnel(metalness, halfwayVector, viewDir);
	float geometry = GeometryFunction(normal, halfwayVector, viewDir, lightDir);

	vec3 diffuse = (subpassLoad(inputAlbedo).rgb / PI) * (vec3(1.0f) - fresnel) * (1 - metalness);

	float brdf_denom = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);

	return diffuse + ((ndf * fresnel * geometry) / (0.001f + brdf_denom));
}

void main() {
	float depth = subpassLoad(inputDepth).x;

	if (depth == 1.0f) discard;

	vec3 pos = posFromDepth(depth);

	vec3 viewDir = normalize(sceneUbo.position.xyz - pos);
	vec3 normal = normalize(subpassLoad(inputNormals).xyz);

	vec3 ambient = vec3(0.03f) * subpassLoad(inputAlbedo).xyz;
	vec3 colour = vec3(0.0f);

	for (int i = 0; i < MAX_LIGHTS; i++) {
		vec3 lightCol = lights.light[i].color.rgb;
		vec3 lightPos = lights.light[i].pos.xyz;
		vec3 lightDir = normalize(lightPos - pos);
		vec3 brdfVal = brdf(lightDir, viewDir, normal) * 100;
		if (i == 0) {
			brdfVal *= 10;
		}
		float NdotL = max(dot(normal, lightDir), 0.0f);
		float attenuation = 1 / pow(length(lightPos - pos), 2);

		colour += (ambient * brdfVal * lightCol * NdotL) * attenuation;
	}

	colour = mix(colour, colour * subpassLoad(inputEmissive).a, 1.0f);

	colour += subpassLoad(inputEmissive).rgb;

	oColor = vec4(colour, 1.0f);
}