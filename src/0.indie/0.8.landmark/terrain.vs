#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 trans;
layout (location = 2) in vec4 neighbour;

uniform sampler2DRect worldMap;
uniform float scale;

out vec4 isNeigh;

void main()
{
	gl_Position = vec4(pos.x * trans.z + trans.x, 0.,pos.z * trans.z + trans.y, 1.0f);
	isNeigh = neighbour;
}