#version 330 core

out vec4 fragColor;

in vec2 TexCoord;
in vec4 vertexColor;

// Tex object
uniform sampler2D ourTex;

void main()
{
    fragColor = texture(ourTex, TexCoord);
}