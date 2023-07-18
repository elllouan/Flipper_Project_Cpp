#version 330 core

out vec4 fragColor;

in vec2 TexCoord;
in vec4 vertexColor;

// Texture object
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // 80% of the first texture + 20% of the second one
    fragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5) * vertexColor;
}