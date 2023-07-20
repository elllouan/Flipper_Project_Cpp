#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 TexCoord;

out vec4 vertexColor;
out vec2 Tex1Coord;
out vec2 Tex2Coord;

uniform float diagTranslation;
uniform mat4 transform;

void main()
{
	// gl_Position = vec4(position.x + diagTranslation, position.y + diagTranslation, position.zw);
    gl_Position = transform * position;
    vertexColor = color;
    Tex1Coord = vec2(TexCoord.s, TexCoord.t);
    Tex2Coord = vec2(TexCoord.s, TexCoord.t);
}