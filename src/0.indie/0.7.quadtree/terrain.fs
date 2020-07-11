#version 330
uniform sampler2D waterTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

in vec4 texWeights;
in vec2 uv;
in vec3 fragPos;
in float isWater;

uniform float scale;
uniform int normalScale;
uniform sampler2DRect normalMap;


uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 aTangent;
uniform vec3 aNormal;
uniform vec3 aBitangent;

void main()
{

    // normal
    vec3 normal = texture(normalMap, uv * scale / normalScale).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // ambient
    
    vec3 color = texture(waterTexture, uv * 5.).rgb * texWeights[0] 
        + texture(grassTexture, uv * 5.).rgb * texWeights[1]
        + texture(rockTexture, uv * 5.).rgb * texWeights[2]
        + texture(snowTexture, uv * 5.).rgb * texWeights[3];
    
    //vec3 riversColor = texture(riversMap, uv * scale).rgb;
    vec3 ambient = 0.2 * color;

    // tangent space
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    vec3 TangentLightPos = TBN * lightPos;
    vec3 TangentViewPos  = TBN * viewPos;
    vec3 TangentFragPos  = TBN * fragPos;


    // diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
	//gl_FragColor = vec4(0.3,0.3,0.3,1.0);
}