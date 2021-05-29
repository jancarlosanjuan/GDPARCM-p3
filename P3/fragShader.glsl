#version 430

in vec3 f_color;
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in vec4 FragPosLightSpace;

uniform vec3 u_lightColor;
uniform vec3 u_lightDir;
uniform vec3 u_viewPos;
uniform sampler2D texture1;
uniform sampler2D u_shadowMap;

out vec4 fragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(u_lightDir - fragPos);
    float bias = max(0.0015 * (1.0 - dot(norm, lightDir)), 0.0005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main(void)
{    
    vec3 color = texture(texture1, texCoord).rgb;
    float ambientStrength = 1.1f; //0.1
    vec3 ambient = ambientStrength * color;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(u_lightDir - fragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * u_lightColor;

    vec3 viewDir = normalize(u_viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64);
    vec3 specular = spec * u_lightColor;  

    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    fragColor = vec4(lighting, 1.0);
}