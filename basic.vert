#version 330 core

layout(location = 0) in vec3 aPos;    // Pozicija
layout(location = 1) in vec2 aTex;    // Tekstura
layout(location = 2) in vec3 aNormal; // Normala (za svetlo)

out vec2 chTex;
out vec3 chNormal;  // Saljemo normalu u Fragment shader
out vec3 chFragPos; // Saljemo poziciju piksela u svetu

uniform mat4 uM;
uniform mat4 uV;
uniform mat4 uP;

void main()
{
    chTex = aTex;
    
    // Racunamo stvarnu poziciju
    chFragPos = vec3(uM * vec4(aPos, 1.0));
    
    // Racunamo normalu
    chNormal = mat3(transpose(inverse(uM))) * aNormal;
    
    gl_Position = uP * uV * vec4(chFragPos, 1.0);
}