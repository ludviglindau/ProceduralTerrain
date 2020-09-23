#version 450
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(rgba16f, binding = 0) uniform image2D outTexture;

layout(location = 0) uniform float seed;

vec2 random2(vec2 p)
{
	p = mod(abs(p), 16.0f);
    p = vec2(dot(p, vec2(905.0, 311.365)),
             dot(p, vec2(684.63, 866.28)));
    return -1.0 + 2.0 * fract(sin(p) * 12583.3602648 + ((seed + 0.3782111) * 155.155));
}

vec3 noise(vec2 p)
{
    vec2 v = floor(p);
    vec2 w = fract(p);
    vec2 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    vec2 du = 30.0*w*w*(w*(w-2.0)+1.0);

    vec2 a = vec2(0.0, 0.0);
    vec2 b = vec2(1.0, 0.0);
    vec2 c = vec2(0.0, 1.0);
    vec2 d = vec2(1.0, 1.0);

    vec2 ga = random2(v + a);
    vec2 gb = random2(v + b);
    vec2 gc = random2(v + c);
    vec2 gd = random2(v + d);

    float va = dot(ga, w - a);
    float vb = dot(gb, w - b);
    float vc = dot(gc, w - c);
    float vd = dot(gd, w - d);

    return vec3(va + u.x*(vb-va) + u.y*(vc-va) + u.x*u.y*(va-vb-vc+vd),   // value
                ga + u.x*(gb-ga) + u.y*(gc-ga) + u.x*u.y*(ga-gb-gc+gd) +  // derivatives
                du * (u.yx*(va-vb-vc+vd) + vec2(vb,vc) - va));
}

vec3 fbm(vec2 p, int octaves)
{
    vec3 value = vec3(0.0);
    float freq = 0.5f;
    float amp = 2.0f;

    for (int i = 0; i < octaves; ++i) {
        value += noise(p * freq) * amp * vec3(1.0, freq, freq);
        freq *= 2.0;
        amp *= 0.5;
    }

    return value;
}

void main()
{
	float dt = 1.0 / 60.0;
    vec2 p = gl_GlobalInvocationID.xy;
    vec4 density = vec4(fbm(p / 128.f, 8), 0.0);
    imageStore(outTexture, ivec2(p), density);
}
