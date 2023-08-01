#version 330 core

out vec4 fragColor;

in vec2 Tex1Coord;
in vec2 Tex2Coord;

// Texture object
uniform sampler2D woodSampler;
uniform sampler2D smileySampler;

void main()
{
    // 80% of the first texture + 20% of the second one
    fragColor = mix(texture(woodSampler, Tex1Coord), texture(smileySampler, Tex2Coord), 0.2);
}