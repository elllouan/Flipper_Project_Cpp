#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

out vec4 vertexColor;
out vec2 TexCoord;

uniform float diagTranslation;

void main()
{
	gl_Position = vec4(position.x + diagTranslation, position.y + diagTranslation, position.zw);
    vertexColor = color;
    TexCoord = texCoord;
}