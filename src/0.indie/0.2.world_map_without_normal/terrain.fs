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
uniform sampler2DRect normalMap;
uniform sampler2DRect terrainMap;
uniform sampler2DRect riversMap;

uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 aTangent;
uniform vec3 aNormal;
uniform vec3 aBitangent;

void main()
{

    // normal
    //vec3 normal = texture(normalMap, uv * scale / 2.0).rgb;
    //normal = normalize(normal * 2.0 - 1.0);

	const vec2 size = vec2(2.0,0.0);
    const ivec3 off = ivec3(-1,0,1);

    float s01 = textureOffset(terrainMap, uv * 10., off.xy).x;
    float s21 = textureOffset(terrainMap, uv * 10., off.zy).x;
    float s10 = textureOffset(terrainMap, uv * 10., off.yx).x;
    float s12 = textureOffset(terrainMap, uv * 10., off.yz).x;

	//1st method 
    //vec3 va = normalize(vec3(size.xy,s21-s01));
    //vec3 vb = normalize(vec3(size.yx,s12-s10));
    //vec3 normal = cross(va,vb);

	//2nd method
	vec3 normal = normalize(vec3((s01-s21)/2./1.5,(s10-s12)/2./1.5,1));

    // ambient
    
//    vec3 color = texture(waterTexture, uv * 10.).rgb * texWeights[0] 
//        + texture(grassTexture, uv * 10.).rgb * texWeights[1]
//        + texture(rockTexture, uv * 10.).rgb * texWeights[2]
//        + texture(snowTexture, uv * 10.).rgb * texWeights[3];
    vec3 terrainColor = texture(terrainMap, uv * scale).rgb;
    vec3 riversColor = texture(riversMap, uv * scale).rgb;
    vec3 color;
    if (riversColor.r < 0.1 || riversColor.g < 0.1 || riversColor.g <0.1) {
         color = riversColor;
    } else {
         color = terrainColor;
    }
    vec3 ambient = 0.1 * color;

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
	//gl_FragColor = vec4((normal +1.0)/2.0, 1.0);
}