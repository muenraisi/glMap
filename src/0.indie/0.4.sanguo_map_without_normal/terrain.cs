#version 400

layout(vertices = 4) out;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform float scale;

uniform sampler2DRect deltaMap;

float lodEdge(vec4 pointA, vec4 pointB)
{
    vec4 clipA = mvpMatrix * pointA;
    vec4 clipB = mvpMatrix * pointB;

    vec3 ndcA = clipA.xyz / clipA.w;
    vec3 ndcB = clipB.xyz / clipB.w;

    if ((abs(ndcA.x) > 1 || abs(ndcA.z) > 1) && (abs(ndcB.x) > 1 || abs(ndcB.z) > 1)) // both of the two point out of the NDC range
        return 1.0;

    float absDistance = sqrt((ndcA.x - ndcB.x) * (ndcA.x - ndcB.x) + (ndcA.z - ndcB.z) * (ndcA.z - ndcB.z));

    float deltaA = texture(deltaMap, vec2(pointA.x / 10, pointA.z / 10) * scale).x;
    float deltaB = texture(deltaMap, vec2(pointB.x / 10, pointB.z / 10) * scale).x;
    float delta = deltaA>deltaB?deltaA:deltaB;

    float level = absDistance * delta*delta * 256;
    if (level > 1) return level;
    else return 1;
    
}

void main()
{

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = lodEdge(gl_in[0].gl_Position, gl_in[3].gl_Position);
    gl_TessLevelOuter[1] = lodEdge(gl_in[1].gl_Position, gl_in[0].gl_Position);
    gl_TessLevelOuter[2] = lodEdge(gl_in[2].gl_Position, gl_in[1].gl_Position);
    gl_TessLevelOuter[3] = lodEdge(gl_in[3].gl_Position, gl_in[2].gl_Position);
    gl_TessLevelInner[1] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]) / 2.0;
    gl_TessLevelInner[0] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]) / 2.0;

}
