#version 400

layout(quads, equal_spacing, ccw) in;
uniform sampler2DRect worldMap;
uniform float scale;
vec4 posn;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    posn = (1-u)* (1-v) * gl_in[0].gl_Position
         + u * (1-v) * gl_in[1].gl_Position
         + u * v * gl_in[2].gl_Position
         + (1-u) * v * gl_in[3].gl_Position;

	float s = posn.x * scale;
    float t = posn.z * scale;

    posn = vec4(posn.x,texture(worldMap, vec2(s, t)).x  , posn.z, posn.w);
    gl_Position = posn;
}

