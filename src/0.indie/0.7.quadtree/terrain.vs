#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aOffset;

void main()
{
	gl_Position = vec4(pos.x*aOffset.z+aOffset.x, pos.y, pos.z*aOffset.z+aOffset.y, 1.0);
	//gl_Position = vec4(pos.x*aOffset.z+aOffset.x, pos.y, pos.z*aOffset.z+aOffset.y, 1.0);
}