#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform float u_time;
uniform float u_speed;
uniform float u_intensity; // overall brightness
uniform float u_layerScale;
uniform float u_softness;  // edge softness
uniform float u_glow;      // extra bloom multiplier
uniform int u_layers;      // number of ribbon layers
uniform vec3 u_colorA;     // deep red
uniform vec3 u_colorB;     // dark orange

// Enhanced hash function for cleaner float distributions
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

// Smooth value noise
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// FIXED: Dropped iterations from 5 to 3 to eliminate the jagged, pixelated zig-zag spikes
float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    for (int i = 0; i < 3; ++i) {
        v += a * noise(p);
        p *= 2.0;
        a *= 0.5;
    }
    return v;
}

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
    vec2 pos = uv;
    pos.x *= u_resolution.x / u_resolution.y;

    vec3 accum = vec3(0.0);
    float t = u_time * u_speed;
    int layers = max(1, u_layers);

    for (int i = 0; i < 12; ++i) {
        if (i >= layers) break;
        float li = float(i);

        // layer-based parameters
        float scale = mix(0.5, 2.0, fract(li * 0.37)) * u_layerScale;
        float amp = mix(0.05, 0.20, fract(li * 0.51));
        float freq = mix(1.0, 2.8, fract(li * 0.29));
        float speedOff = mix(0.5, 1.3, fract(li * 0.73));

        // compute a smooth centerline using fbm
        float x = pos.x * scale + li * 10.0;
        float n = fbm(vec2(x * 0.6, t * 0.08 * speedOff));

        // Smooth undulating sine wave line with increased vertical separation
        float center = 0.5 + sin(pos.x * freq + n * 4.0 + t * 0.2 * speedOff) * amp + (li - float(layers)/2.0) * 0.05;

        // Ribbon shape vertical falloff math
        float width = mix(0.04, 0.12, fract(li * 0.21));
        float d = abs(pos.y - center);

        // Smooth step width calculation produces an edge mask in [0..1]
        float layerAlpha = 1.0 - smoothstep(width, width * (1.0 - u_softness), d);

        // Normalized layer visibility dropoff and intensity
        layerAlpha *= (1.0 - (li / float(layers)) * 0.4) * u_intensity;

        // Density-based color blending: edges -> u_colorA, thick centers -> u_colorB
        float blend = mix(0.0, 1.0, layerAlpha);
        vec3 layerColor = mix(u_colorA, u_colorB, blend);

        // Modulated internal smoky glow (keeps color tonal)
        float internalGlow = fbm(vec2(pos.x * 1.0 + li * 1.5, pos.y * 0.5 + t * 0.1));
        layerColor *= 0.5 + 0.5 * internalGlow;

        // Accumulate with a controlled factor to avoid clipping
        accum += layerColor * layerAlpha * 0.4;
    }

    vec3 color = accum;
    
    // FIXED: Controlled soft bloom component that preserves underlying colors without forcing them to pure white
    float brightness = length(color);
    color += color * pow(brightness, 2.0) * (u_glow * 0.5);
    
    // Final clamp to ensure safe color space constraints
    color = clamp(color, 0.0, 1.0);

    // Vignette logic (1.0 in center, drops off smoothly at screen edges)
    float distFromCenter = distance(uv, vec2(0.5));
    float vig = smoothstep(0.8, 0.35, distFromCenter);
    color *= vig;

    gl_FragColor = vec4(color, 1.0);
}