#version 450

#define PI 3.14159265359

layout(location = 0) in vec3 outPosition;
layout(location = 1) in vec3 outNormal;
layout(location = 2) in vec2 outTexCoord;
layout(location = 3) in vec4 outVertexColour;

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

layout(location = 0) out vec4 oColor;

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
    vec3 f0 = (1 - metalness) * vec3(0.04) + (metalness * texture(baseColourMap, outTexCoord).rgb);
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

    float metalness = texture(metallicRoughness, outTexCoord).g;
    float roughness_sqrt = texture(metallicRoughness, outTexCoord).b;
    float roughness = roughness_sqrt * roughness_sqrt;

    float ndf = DistributionFunction(normal, halfwayVector, roughness);
    vec3 fresnel = Fresnel(metalness, halfwayVector, viewDir);
    float geometry = GeometryFunction(normal, halfwayVector, viewDir, lightDir);    

    vec3 diffuse = (texture(baseColourMap, outTexCoord).rgb / PI) * (vec3(1.0f) - fresnel) * (1 - metalness);

    float brdf_denom = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);

    return diffuse + ((ndf * fresnel * geometry) / (0.001f + brdf_denom));
}

void main() {
    vec3 lightCol = vec3(1.0f);
    vec3 lightPos = vec3(2.0f, 5.0f, 0.0f);

    vec3 lightDir = normalize(lightPos - outPosition);
    vec3 viewDir = normalize(sceneUbo.position.rgb - outPosition);
    vec3 normal = normalize(outNormal);

    vec3 ambient = vec3(0.03f) * texture(baseColourMap, outTexCoord).rgb;
    
    oColor = vec4(ambient + (brdf(lightDir, viewDir, normal) * 100) * lightCol * (max(dot(normal, lightDir), 0.0)), 0.0f) / pow(length(lightPos - outPosition), 2);
}