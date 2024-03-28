#version 330 core

in vec2 texCoords;

out vec4 outColor;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);
    outColor = vec4(textColor, 1.0) * sampled;
}

