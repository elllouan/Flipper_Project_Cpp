#version 330 core

out vec4 fragColor;

in vec2 Tex1Coord;
in vec2 Tex2Coord;

// Texture object
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // 80% of the first texture + 20% of the second one
    fragColor = mix(texture(texture1, Tex1Coord), texture(texture2, Tex2Coord), 0.3);
}