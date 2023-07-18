#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 TexCoord;

out vec4 vertexColor;
out vec2 Tex1Coord;
out vec2 Tex2Coord;

uniform float diagTranslation;

void main()
{
	gl_Position = vec4(position.x + diagTranslation, position.y + diagTranslation, position.zw);
    vertexColor = color;
    Tex1Coord = vec2(TexCoord.s*2, TexCoord.t*2);
    Tex2Coord = vec2(TexCoord.s*4, TexCoord.t*4);
}