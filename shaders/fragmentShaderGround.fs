#version 330 core

out vec4 ground;

in vec2 TexCoord;

uniform sampler2D groundSampler;

void main()
{
    ground = texture(groundSampler, TexCoord);
}