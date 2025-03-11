#version 450

layout(location = 0) in vec3 v2fNormal;
layout(location = 1) in vec4 v2fTangent;
layout(location = 2) in vec2 v2fTexCoord0;
layout(location = 3) in vec2 v2fTexCoord1;
layout(location = 4) in vec4 v2fVertexColour;

layout(set = 0, binding = 0) uniform SceneUBO {
	mat4 projection;
	mat4 view;
	vec4 position;
} sceneUbo;

layout(set = 1, binding = 0) uniform sampler2D baseColourMap;
layout(set = 1, binding = 1) uniform sampler2D metallicRoughness;
layout(set = 1, binding = 2) uniform sampler2D emissiveMap;
layout(set = 1, binding = 3) uniform sampler2D occlusionMap;
layout(set = 1, binding = 4) uniform sampler2D normalMap;

struct MaterialInfo {
    vec4 emissiveFactor;
    vec4 baseColourFactor;
    int alphaMode;
    int emissiveTexSet;
    int baseColorTexSet;
    int normalTexSet;
    int metallicRoughnessTexSet;
    int occlusionTexSet;
    float alphaCutoff;
    float emissiveStrength;
    float occlusionStength;
    float metallicFactor;
    float roughnessFactor;
};

layout(std430, set = 2, binding = 0) readonly buffer MaterialInfoSSBO {
    MaterialInfo materialInfo[];
};

layout(push_constant) uniform PushConstants {
    int materialIndex;
} pushConstants;

layout(location = 0) out vec4 outNormals;  // rgb = normals,  a = metalness
layout(location = 1) out vec4 outAlbedo;   // rgb = albedo,   a = roughness
layout(location = 2) out vec4 outEmissive; // rgb = emissive, a = occlusion

// Credit: https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/data/shaders/includes/srgbtolinear.glsl
vec4 SRGBtoLINEAR(vec4 srgbIn)
{
	#define MANUAL_SRGB 1
	#ifdef MANUAL_SRGB
	#ifdef SRGB_FAST_APPROXIMATION
	vec3 linOut = pow(srgbIn.xyz,vec3(2.2));
	#else //SRGB_FAST_APPROXIMATION
	vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
	vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
	#endif //SRGB_FAST_APPROXIMATION
	return vec4(linOut,srgbIn.w);;
	#else //MANUAL_SRGB
	return srgbIn;
	#endif //MANUAL_SRGB
}

vec3 getNormal() {
	vec3 tangentNormal = texture(normalMap, v2fTexCoord0).xyz * 2.0f - 1.0f;

	vec3 N = normalize(v2fNormal);
	vec3 T = normalize(v2fTangent.xyz);
	vec3 B = normalize(cross(N, T) * v2fTangent.w);
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

void main() {
    MaterialInfo matInfo = materialInfo[pushConstants.materialIndex];

	vec3 normal = getNormal();
	outNormals.rgb = normal;

    float metallicFactor = matInfo.metallicFactor;
    float roughnessFactor = matInfo.roughnessFactor;

    vec2 metallicRoughnessTexCoords;
    if (matInfo.metallicRoughnessTexSet == 0) {
        metallicRoughnessTexCoords = v2fTexCoord0;
    } else {
        metallicRoughnessTexCoords = v2fTexCoord1;
    }

    float metalness = texture(metallicRoughness, metallicRoughnessTexCoords).b * metallicFactor;
    float roughness_sqrt = texture(metallicRoughness, metallicRoughnessTexCoords).g * roughnessFactor;
    float roughness = roughness_sqrt * roughness_sqrt;

    outNormals.a = metalness;
    outAlbedo.a = roughness;

    vec4 albedo;
    vec3 a;
    if (matInfo.baseColorTexSet > -1) {

        vec2 baseColorTexCoords;
        if (matInfo.baseColorTexSet == 0) {
            baseColorTexCoords = v2fTexCoord0;
        } else {
            baseColorTexCoords = v2fTexCoord1;
        }
    
        albedo = SRGBtoLINEAR(texture(baseColourMap, baseColorTexCoords)) * matInfo.baseColourFactor;
        a = texture(baseColourMap, v2fTexCoord0).rgb;
    
    } else {
        albedo = matInfo.baseColourFactor;
    }

    albedo *= v2fVertexColour;

    vec2 occlusionTexCoords;
    if (matInfo.occlusionTexSet > -1) {
        float ambientOcclusion = texture(occlusionMap, matInfo.occlusionTexSet == 0 ? v2fTexCoord0 : v2fTexCoord1).r;
        outEmissive.a = ambientOcclusion;
    }

    vec3 emissive = matInfo.emissiveFactor.rgb * matInfo.emissiveStrength;
    if (matInfo.emissiveTexSet > -1) {
        emissive *= SRGBtoLINEAR(texture(emissiveMap, matInfo.emissiveTexSet == 0 ? v2fTexCoord0 : v2fTexCoord1)).rgb;
    }

    outEmissive.rgb = emissive;

    outAlbedo.rgb = a.rgb;
}