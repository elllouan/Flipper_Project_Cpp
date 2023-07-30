#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 texCoord;

out vec2 Tex1Coord;
out vec2 Tex2Coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;


void main()
{
    gl_Position = perspective * view * model * position;
    Tex1Coord = vec2(texCoord.s, texCoord.t);
    Tex2Coord = vec2(texCoord.s, texCoord.t);
}