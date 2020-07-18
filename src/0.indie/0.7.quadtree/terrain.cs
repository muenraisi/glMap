#version 400

layout(vertices = 4) out;
in vec4 isNeigh[];

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform vec3 viewPos;

float lodEdge(vec3 pointA, vec3 pointB, float split)
{
    float d = distance(pointA, pointB);
    vec3 mid = (pointA + pointB) / 2.;
    float l = distance(mid, viewPos);
    return max(pow(2, ceil ( log2(l/ d/100))), split+1);
    //return split + 1.;

}

void main()
{

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = lodEdge(gl_in[0].gl_Position.xyz, gl_in[3].gl_Position.xyz, isNeigh[0].z);
    gl_TessLevelOuter[1] = lodEdge(gl_in[1].gl_Position.xyz, gl_in[0].gl_Position.xyz, isNeigh[0].y);
    gl_TessLevelOuter[2] = lodEdge(gl_in[2].gl_Position.xyz, gl_in[1].gl_Position.xyz, isNeigh[0].x);
    gl_TessLevelOuter[3] = lodEdge(gl_in[3].gl_Position.xyz, gl_in[2].gl_Position.xyz, isNeigh[0].w);
    gl_TessLevelInner[1] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]) / 2.0;
    gl_TessLevelInner[0] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]) / 2.0;

    //gl_TessLevelOuter[0] = 1;
    //gl_TessLevelOuter[1] = 1;
    //gl_TessLevelOuter[2] = 1;
    //gl_TessLevelOuter[3] = 1;
    //gl_TessLevelInner[1] = 1;
    //gl_TessLevelInner[0] = 1;
}
