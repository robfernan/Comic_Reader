// Fragment shader to generate a PSP-like flame background.
// Uniforms: u_resolution (vec2), u_time (float)

uniform vec2 u_resolution;
uniform float u_time;
uniform float u_speed;
uniform float u_intensity;
uniform float u_layerScale;
uniform float u_glow;
uniform vec3 u_colorA;
uniform vec3 u_colorB;

// Simple pseudo-random
float hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1,311.7)), dot(p, vec2(269.5,183.3)));
    return fract(sin(p.x+p.y) * 43758.5453123);
}

// 2D noise
float noise(in vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    // Four corners
    float a = hash(i + vec2(0.0,0.0));
    float b = hash(i + vec2(1.0,0.0));
    float c = hash(i + vec2(0.0,1.0));
    float d = hash(i + vec2(1.0,1.0));
    vec2 u = f*f*(3.0-2.0*f);
    return mix(a, b, u.x) + (c - a)*u.y*(1.0-u.x) + (d - b)*u.x*u.y;
}

// Fractal Brownian Motion
float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100);
    for (int i = 0; i < 5; ++i) {
        v += a * noise(p);
        p = p*2.0 + shift;
        a *= 0.5;
    }
    return v;
}

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
    vec2 p = uv * vec2(u_resolution.x/u_resolution.y, 1.0);

    // Layered ribbon generation
    const int LAYERS = 6;
    float accum = 0.0;
    vec3 accumCol = vec3(0.0);

    // parameters controlling ribbon look
    float baseThickness = 0.05; // base thickness of ribbons
    float baseFreq = 1.2 * u_layerScale;
    float baseAmp = 0.06;

    for (int i = 0; i < LAYERS; ++i) {
        float fi = float(i);
        // vertical position for the ribbon band (spread out vertically)
        float bandCenter = 0.5 + (fi - (LAYERS-1.0)/2.0) * 0.11;

        // per-layer motion and waviness
        float freq = baseFreq * (0.8 + fi*0.18);
        float amp = baseAmp * (1.0 + fi*0.25);
        float speed = u_speed * (0.6 + fi*0.2);
        float jitter = fbm(vec2(p.x*1.8 + fi*2.2, u_time*0.25 + fi*2.0))*0.1;

        // compute ribbon center y using sin wave + noise
        float centerY = bandCenter + sin(p.x*freq + u_time*speed + fi*1.9)*amp + jitter;

        // thickness tapers depending on layer
        float thickness = baseThickness * (1.0 - fi*0.07);

        float d = abs(uv.y - centerY);
        float edge = smoothstep(thickness*0.9, thickness, d);
        // intensity stronger near center and attenuate with vertical position
        float intensityMask = (1.0 - edge) * (1.0 - abs(bandCenter - 0.5)*0.8);

        // color gradient per layer
        vec3 col = mix(u_colorA, u_colorB, fi / float(LAYERS-1));

        // accumulate additively
        accum += intensityMask;
        accumCol += col * intensityMask;
    }

    // tone and glow
    vec3 overlay = accumCol * u_glow * u_intensity;
    overlay = pow(overlay, vec3(1.0/1.05));

    // subtle horizontal streaks to add detail
    float streak = fbm(vec2(p.x*9.0 + u_time*0.9, p.y*0.5));
    overlay += vec3(0.06,0.02,0.0) * pow(streak, 3.0) * (1.0 - uv.y) * u_glow;

    // vignette
    float dx = uv.x - 0.5;
    float dy = uv.y - 0.5;
    float dist = sqrt(dx*dx + dy*dy);
    float vign = smoothstep(0.9, 0.35, dist);
    overlay *= (1.0 - 0.7 * vign);

    // final alpha based on accumulated intensity
    float alpha = clamp(accum*0.45, 0.0, 1.0);
    gl_FragColor = vec4(overlay, alpha);
}
