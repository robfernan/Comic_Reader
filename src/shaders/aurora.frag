#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform float u_time;
uniform float u_speed;
uniform float u_intensity; 
uniform float u_layerScale;
uniform float u_softness;  
uniform float u_glow;      
uniform int u_layers;      

// Classic noise hashing
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

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

// 3-Octave FBM eliminates jagged micro-spikes
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

    // HARD LOCKED PALETTE: Bypassing uniform configuration issues completely
    vec3 colorA = vec3(0.29, 0.05, 0.05); // Deep wine red
    vec3 colorB = vec3(0.85, 0.29, 0.09); // Intense glowing orange

    // CRITICAL PSP FIX: Force the base canvas starting point to absolute pitch black
    vec3 accumColor = vec3(0.0);
    
    float t = u_time * u_speed;
    int layers = max(1, u_layers);

    for (int i = 0; i < 12; ++i) {
        if (i >= layers) break;
        float li = float(i);
        
        // Dynamic layer variation mechanics
        float scale = mix(0.6, 1.8, fract(li * 0.37)) * u_layerScale;
        float amp = mix(0.06, 0.16, fract(li * 0.51)); 
        float freq = mix(1.2, 2.6, fract(li * 0.29));
        float speedOff = mix(0.5, 1.2, fract(li * 0.73));
        
        // Generate flowing wave pathing
        float x = pos.x * scale + li * 15.0;
        float n = fbm(vec2(x * 0.5, t * 0.06 * speedOff));
        
        // Vertical dispersion spacing multiplier set to 0.05 for independent ribbon sheets
        float center = 0.5 + sin(pos.x * freq + n * 4.5 + t * 0.22 * speedOff) * amp + (li - float(layers)/2.0) * 0.05;
        
        float width = mix(0.03, 0.09, fract(li * 0.21));
        float d = abs(pos.y - center);
        
        // Ribbon profile vertical alpha drop-off
        float layerAlpha = smoothstep(width, width * (1.0 - u_softness), d);
        layerAlpha *= (1.0 - (li / float(layers)) * 0.3) * u_intensity;
        
        // DENSITY BLENDING: Transparent wispy edges = Red, thick dense cores = Glowing Orange
        float colorBlendFactor = smoothstep(0.1, 0.8, layerAlpha);
        vec3 layerColor = mix(colorA, colorB, colorBlendFactor);
        
        // Internal structural smoky details
        float internalSmoke = fbm(vec2(pos.x * 1.3 + li * 2.0, pos.y * 0.6 + t * 0.12));
        layerColor *= 0.4 + 0.6 * internalSmoke;

        // Add colors strictly inside the ribbon boundaries over the dark background
        accumColor += layerColor * layerAlpha * 0.4;
    }

    vec3 finalColor = accumColor;
    
    // Smooth backlit bloom component
    float brightness = length(finalColor);
    finalColor += finalColor * pow(brightness, 2.0) * (u_glow * 0.4);
    finalColor = clamp(finalColor, 0.0, 1.0);

    // Screen vignette to ensure edges taper off entirely to black
    float distFromCenter = distance(uv, vec2(0.5));
    float vig = smoothstep(0.85, 0.4, distFromCenter);
    finalColor *= vig;

    gl_FragColor = vec4(finalColor, 1.0);
}