#version 450

in vec3 worldPos;
in vec3 normal;

out vec4 fragColor;

layout(location = 0) uniform ivec2 mapSize;
layout(location = 1) uniform ivec2 mapResolution;
layout(location = 2) uniform mat4 viewProj;
layout(location = 3) uniform vec3 cameraPos;
layout(location = 4) uniform float time;
layout(location = 5) uniform float height;

layout(binding = 0) uniform sampler2D heightmap;

#define GROUND_COLOR vec3(0.25, 0.35, 0.20)
#define HILL_COLOR vec3(0.35, 0.15, 0.10)
#define WATER_COLOR vec3(0.025, 0.1, 0.2)
#define SHORE_COLOR vec3(0.5, 0.5, 0.5)
#define FOG_COLOR vec3(0.4, 0.5, 0.6)
#define WATER_LEVEL 0.01f
#define SHORE_LEVEL 0.02f

void main()
{
	float t = time;
	vec3 lightPos = vec3(150.0, 500.0, 200.0);
    vec3 lightDir = normalize(lightPos - worldPos);
	float attenuation = smoothstep(200000.0, 0.0, distance(lightPos, worldPos));
	vec3 n = normalize(normal);
	
	//albedo & ambient
	fragColor.rgb += mix(GROUND_COLOR, HILL_COLOR, worldPos.y / height);
	fragColor.rgb = worldPos.y < SHORE_LEVEL ? SHORE_COLOR : fragColor.rgb;
	fragColor.rgb = worldPos.y < WATER_LEVEL ? WATER_COLOR : fragColor.rgb;
	fragColor.rgb += max(dot(n, vec3(0.0, 1.0, 0.0)), 0.0) * vec3(0.0, 0.2, 0.05);
	
	//diffuse
    fragColor.rgb = fragColor.rgb * (max(dot(lightDir, n), 0.25)) * attenuation;

	//specular
	vec3 halfway = normalize(normalize(cameraPos - worldPos.xyz) + lightDir);
	if (worldPos.y < WATER_LEVEL) {
		const vec3 specularColor = vec3(0.3);
		vec3 specularReflection = vec3(0.0);
		specularReflection.rgb += pow(max(dot(n, halfway), 0.0), 2.0) * specularColor.rgb * 0.25;
		specularReflection.rgb += pow(max(dot(n, halfway), 0.0), 4.0) * specularColor.rgb * 0.25;
		specularReflection.rgb += pow(max(dot(n, halfway), 0.0), 16.0) * specularColor.rgb * 0.25;
		specularReflection.rgb += pow(max(dot(n, halfway), 0.0), 256.0) * specularColor.rgb * 0.25;

		fragColor.rgb += specularReflection * attenuation;
	}
	
	// fog
	vec3 worldToCamera = cameraPos - worldPos.xyz;
	float fogDensity = exp(-length(worldToCamera) * 0.001) / exp(-worldPos.y * 0.01);
	float heightFactor = exp(-texture(heightmap, (worldPos.xz + vec2(time * 10.0, 0.0)) / mapResolution).r * 1.5f) * 0.25;
	fogDensity /= max(heightFactor, 1.0);
	fogDensity = clamp(fogDensity, 0.0, 1.0);
	float sunAmount = pow(max(dot(vec3(0.0, 1.0, 0.0), halfway), 0.0), 8.0);
	vec3 fogColor = mix(FOG_COLOR, vec3(0.8, 0.8, 0.8), sunAmount);
	fragColor.rgb = mix(fragColor.rgb, fogColor, (1.0 - fogDensity));
}
