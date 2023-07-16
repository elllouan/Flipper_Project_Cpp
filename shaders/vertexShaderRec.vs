#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 vertexColor;

uniform vec2 rotate2D;

void main()
{
	gl_Position = vec4(rotate2D.x*position.x + rotate2D.y*position.y, - rotate2D.y*position.x + rotate2D.x*position.y, 0.5f, 1.0f);
    vertexColor = color;
}