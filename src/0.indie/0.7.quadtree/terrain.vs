#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 trans;
layout (location = 2) in vec4 neighbour;
layout (location = 3) in vec4 height;

uniform sampler2DRect worldMap;
uniform float scale;

out vec4 isNeigh;

void main()
{
    float posx = pos.x * trans.z + trans.x;
	float posz = pos.z * trans.z + trans.y;
	gl_Position = vec4(posx, texture(worldMap, vec2(posx * scale, posz * scale)).x/1., posz, 1.0);
	isNeigh = neighbour;
}