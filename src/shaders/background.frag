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

    // Procedural flame overlay (alpha indicates intensity)
    float y = (1.0 - uv.y);
    float n = fbm(vec2(p.x*u_layerScale, p.y*(u_layerScale*0.6) - u_time*u_speed));
    float flame = smoothstep(0.15, 0.75, n * u_intensity + pow(y, 1.5)*1.8);
    vec3 flameCol = mix(u_colorA, u_colorB, clamp((n+0.2)*1.2, 0.0, 1.0));

    // Add subtle horizontal streaks
    float streak = fbm(vec2(p.x*6.0 + u_time*0.6, p.y*0.5));
    vec3 streakCol = vec3(0.05, 0.01, 0.0) * pow(streak, 3.0) * (1.0 - uv.y);

    // Color of overlay
    vec3 overlay = flameCol * flame * u_glow + streakCol * 0.6 * u_glow;

    // Vignette to darken edges of overlay
    float dx = uv.x - 0.5;
    float dy = uv.y - 0.5;
    float dist = sqrt(dx*dx + dy*dy);
    float vign = smoothstep(0.95, 0.3, dist);

    // Slight desaturation
    float gray = dot(overlay, vec3(0.299, 0.587, 0.114));
    overlay = mix(overlay, vec3(gray), 0.03);

    // Output overlay with alpha proportional to flame intensity
    float alpha = clamp(flame*0.85 + 0.1*pow(streak,2.0), 0.0, 1.0) * (1.0 - vign);
    gl_FragColor = vec4(overlay * alpha, alpha);
}
