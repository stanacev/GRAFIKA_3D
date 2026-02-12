#version 330 core

in vec2 chTex;
in vec3 chNormal;
in vec3 chFragPos;

out vec4 outCol;

uniform vec4 uColor;
uniform bool uUseTex;
uniform float uTexScale;

// --- PARAMETRI ZA RUPU ---
uniform bool uRupaAktivna;
uniform vec2 uRupaMin;
uniform vec2 uRupaMax;

// --- SVETLO ---
uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform bool uIsLightSource;

// --- LAMPICA ---
uniform vec3 uLampPos;
uniform vec3 uLampColor;

// --- TEKSTURE ---
uniform sampler2D uTex;
uniform sampler2D uTexNormal;
uniform sampler2D uTexMetallic;
uniform bool uUseNormalMap;

// --- FUNKCIJA ZA RELJEF ---
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(uTexNormal, chTex * uTexScale).xyz * 2.0 - 1.0;
    vec3 Q1  = dFdx(chFragPos);
    vec3 Q2  = dFdy(chFragPos);
    vec2 st1 = dFdx(chTex * uTexScale);
    vec2 st2 = dFdy(chTex * uTexScale);
    vec3 N   = normalize(chNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

// --- FUNKCIJA ZA LAMPICU  ---
vec3 calcPointLight(vec3 lightPos, vec3 lightColor, vec3 normal, vec3 fragPos, vec3 viewDir, float shininess, float metallic, vec3 baseRGB) {
    
    // 1. BLOKADA
    float boxBottomY = 0.5; 
    if (fragPos.y < boxBottomY) {
        return vec3(0.0);
    }

    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // 2. STANDARDNO SVETLO
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseRGB;

    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = 0.5 * spec * lightColor;

    // 3. NOVO: SCATTER / GLOW 
    float distance = length(lightPos - fragPos);
    float glowStrength = 1.0 / (1.0 + 50.0 * distance * distance); 
    vec3 scatter = lightColor * baseRGB * glowStrength * 0.5; // 0.5 je jacina glow-a
    float attenuation = 1.0 / (1.0 + 3.0 * distance + 10.0 * distance * distance);
    float edgeFactor = smoothstep(boxBottomY, boxBottomY + 0.05, fragPos.y);
    return ((diffuse + specular) * attenuation + scatter) * edgeFactor;
}

void main() {
    // 1. RUPA LOGIKA
    if (uRupaAktivna) {
        if (chTex.x > uRupaMin.x && chTex.x < uRupaMax.x &&
            chTex.y > uRupaMin.y && chTex.y < uRupaMax.y) {
            discard;
        }
    }

    // 2. OSNOVNA BOJA
    vec4 baseColor = uColor;
    if(uUseTex) {
        baseColor = texture(uTex, chTex * uTexScale) * uColor;
        if(baseColor.a < 0.1) discard;
    }

    if (uIsLightSource) {
        outCol = baseColor;
        return;
    }

    // 3. MATERIJAL (PBR)
    vec3 norm = normalize(chNormal);
    float roughness = 0.5; 
    float metallic = 0.0;

    if (uUseNormalMap) {
        norm = getNormalFromMap();
        vec4 mr = texture(uTexMetallic, chTex * uTexScale);
        roughness = mr.g; 
        metallic = mr.b;
    }

    // 4. OSVETLJENJE
    vec3 viewDir = normalize(uViewPos - chFragPos);
    float shininess = (1.0 - roughness) * 32.0; 
    if(shininess < 2.0) shininess = 2.0;

    // A) Ambijentalno
    vec3 ambient = 0.3 * uLightColor * baseColor.rgb;

    // B) Glavno svetlo (Plafon)
    vec3 lightDir1 = normalize(uLightPos - chFragPos);
    vec3 halfwayDir1 = normalize(lightDir1 + viewDir);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse1 = diff1 * uLightColor * baseColor.rgb;
    
    vec3 reflectDir = reflect(-lightDir1, norm);
    float spec1 = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular1 = 0.2 * spec1 * uLightColor; 

    // C) Lampica na automatu
    vec3 lampEffect = calcPointLight(uLampPos, uLampColor, norm, chFragPos, viewDir, shininess, metallic, baseColor.rgb);
    lampEffect *= 2.1; 

    vec3 result = ambient + diffuse1 + specular1 + lampEffect;

    outCol = vec4(result, baseColor.a);
}