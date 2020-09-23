#version 450

in vec2 gridPos;

out vec3 worldPos;
out vec3 normal;

layout(location = 0) uniform ivec2 mapSize;
layout(location = 1) uniform ivec2 mapResolution;
layout(location = 2) uniform mat4 viewProj;
layout(location = 4) uniform float time;
layout(location = 5) uniform float height;
layout(binding = 0) uniform sampler2D heightmap;

vec2 random2(vec2 p)
{
    p = vec2(dot(p, vec2(905.0, 311.365)),
             dot(p, vec2(684.63, 866.28)));
    return -1.0 + 2.0 * fract(sin(p) * 12583.3602648);
}

void main()
{
    vec2 offset = vec2((gl_InstanceID % mapSize.x), (gl_InstanceID / mapSize.y));
    worldPos = vec3(gridPos.x + offset.x, 0, gridPos.y + offset.y);
	worldPos.xz += random2(worldPos.xz) * 0.25f; //break grid symmetry a bit

    vec3 heightNormal = texture(heightmap, worldPos.xz / (mapResolution)).rgb;
	if (heightNormal.x <= 0.0f) {
	    heightNormal = texture(heightmap, (worldPos.xz + vec2(time * 1.6, 0.0)) / mapResolution).rgb * 0.1;
		heightNormal.x = 0.000f;
	}
	
    worldPos.y = heightNormal.x * height;
    normal = normalize(vec3(-heightNormal.y, 2.0, -heightNormal.z));
    gl_Position = viewProj * vec4(worldPos, 1.0);
}
