#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

out vec4 vertexColor;
out vec2 TexCoord;

// uniform vec2 rotate2D;

void main()
{
	gl_Position = position;
    vertexColor = color;
    TexCoord = texCoord;
}