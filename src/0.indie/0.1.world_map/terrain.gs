#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 model;
uniform mat4 mvpMatrix;


uniform float waterLevel;
uniform float snowLevel;

out vec4 texWeights;
out vec2 uv;
out vec3 fragPos;
out vec3 normal;
out vec3 debugColor;

float interp(float min, float max, float val)
{
    return (val - min) / (max - min);
}

void calcWeights(float y) {
    if (y < 0.6) {
        texWeights = vec4(0, 1, 0, 0);
    } else if (y < 0.7) {
        float t = interp(0.6, 0.7, y);
        texWeights = vec4(0, 1 - t, t, 0);
    } else {
        texWeights = vec4(0, 0, 1, 0);
    }

    if (y <= waterLevel) {
        texWeights = vec4(1, 0, 0, 0);
    } else if (y >= snowLevel) {
        texWeights = vec4(0, 0, 0, 1);
    } else if (y >= snowLevel - 1) {
        float t = interp(snowLevel - 1, snowLevel, y);
        texWeights[3] = t;
        if (texWeights[1] > 0) {
            texWeights[1] = (1 - t) * texWeights[1];
        }
        if (texWeights[2] > 0) {
            texWeights[2] = (1 - t) * texWeights[2];
        }
    }
}

void main() {

    for (int i = 0; i < gl_in.length(); i++)
    {
        vec4 pos = gl_in[i].gl_Position;


		float y = pos.y;

        if (waterLevel > y) {
			y=waterLevel;
		}
		pos.y = y;
		calcWeights(y);
        
		uv = vec2(pos.x, pos.z);
		fragPos = vec3(model * pos);
        gl_Position = mvpMatrix * pos;
		
		debugColor = vec3(0.3,0.3,0.3);
        EmitVertex();
    }



    EndPrimitive();
}