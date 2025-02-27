#version 450

#define PI 3.14159265359

layout(location = 0) in vec3 outPosition;
layout(location = 1) in vec3 outNormal;
layout(location = 2) in vec4 outTangent;
layout(location = 3) in vec2 outTexCoord0;
layout(location = 4) in vec2 outTexCoord1;
layout(location = 5) in vec4 outVertexColour;

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

layout(location = 0) out vec4 oColor;

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
    vec3 f0 = (1 - metalness) * vec3(0.04) + (metalness * texture(baseColourMap, outTexCoord0).rgb);
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

vec3 brdf(vec3 lightDir, vec3 viewDir, vec3 normal, float metallicFactor, float roughnessFactor) {
    vec3 halfwayVector = normalize(viewDir + lightDir);

    float metalness = texture(metallicRoughness, outTexCoord0).b * metallicFactor;
    float roughness_sqrt = texture(metallicRoughness, outTexCoord0).g * roughnessFactor;
    float roughness = roughness_sqrt * roughness_sqrt;

    float ndf = DistributionFunction(normal, halfwayVector, roughness);
    vec3 fresnel = Fresnel(metalness, halfwayVector, viewDir);
    float geometry = GeometryFunction(normal, halfwayVector, viewDir, lightDir);    

    vec3 diffuse = (texture(baseColourMap, outTexCoord0).rgb / PI) * (vec3(1.0f) - fresnel) * (1 - metalness);

    float brdf_denom = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);

    return diffuse + ((ndf * fresnel * geometry) / (0.001f + brdf_denom));
}

vec3 getNormal() {
    vec3 tangentNormal = texture(normalMap, outTexCoord0).xyz * 2.0f - 1.0f;

    vec3 N = normalize(outNormal);
    vec3 T = normalize(outTangent.xyz);
    vec3 B = normalize(cross(N, T) * outTangent.w);
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    MaterialInfo matInfo = materialInfo[pushConstants.materialIndex];

    vec3 lightCol = vec3(1.0f);
    vec3 lightPos = vec3(0.0f, 3.0f, 0.0f);

    vec3 lightDir = normalize(lightPos - outPosition);
    vec3 viewDir = normalize(sceneUbo.position.rgb - outPosition);
    vec3 normal = getNormal();

    vec4 albedo;
    if (matInfo.baseColorTexSet > -1) {
        albedo = SRGBtoLINEAR(texture(baseColourMap, matInfo.baseColorTexSet == 0 ? outTexCoord0 : outTexCoord1)) * matInfo.baseColourFactor;
    } else {
        albedo = matInfo.baseColourFactor;
    }

    albedo *= outVertexColour;

    float metallicFactor = matInfo.metallicFactor;
    float roughnessFactor = matInfo.roughnessFactor;

    vec3 ambient = vec3(0.03f) * texture(baseColourMap, outTexCoord0).rgb;
    vec3 brdfVal = brdf(lightDir, viewDir, normal, metallicFactor, roughnessFactor) * 100;
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float attenuation = 1 / pow(length(lightPos - outPosition), 2);

    vec3 colour = (ambient * brdfVal * lightCol * NdotL);

    // Occlusion factor
    if (matInfo.occlusionTexSet > -1) {
        float ambientOcclusion = texture(occlusionMap, matInfo.occlusionTexSet == 0 ? outTexCoord0 : outTexCoord1).r;
        colour = mix(colour, colour * ambientOcclusion, 1.0f);
    }

    // Emissive Factor
    vec3 emissive = matInfo.emissiveFactor.rgb * matInfo.emissiveStrength;
    if (matInfo.emissiveTexSet > -1) {
        // glTF says this defines linear multipliers for sampled texels so the queried value might have to be
        // converted from sRGB to linear before multiplying
        emissive *= SRGBtoLINEAR(texture(emissiveMap, matInfo.emissiveTexSet == 0 ? outTexCoord0 : outTexCoord1)).rgb;
    }

    colour += emissive;

    oColor = vec4(colour, 1.0f);
}