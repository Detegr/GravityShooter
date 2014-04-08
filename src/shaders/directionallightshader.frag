layout (location=0) out vec4 color;

uniform vec3 worldTransformation;
uniform vec2 screenSize;
uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D diffuseTex;

vec4 pointLight(vec3 pos, vec3 normal)
{
	float distance = length(pos);
	vec3 direction = normalize(pos);

	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float ambientIntensity = 0.1;
	float diffuseIntensity = 0.0;
	float specularIntensity = 0.0;
	float specularPower = 0.0;

	vec4 ambient = vec4(lightColor, 1.0) * ambientIntensity;
	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);
	float diffuseFactor = dot(normal, -direction);

	if(diffuseFactor > 0)
	{
		diffuse = vec4(lightColor, 1.0) * diffuseIntensity * diffuseFactor;
		vec3 eyeDir = normalize(-pos);
		vec3 reflect = normalize(reflect(direction, normal));
		float specularFactor = dot(eyeDir, reflect);
		specularFactor = pow(specularFactor, 1.0);
		if(specularFactor > 0)
		{
			specular = vec4(lightColor, 1.0) * specularIntensity * specularFactor;
		}
	}
	return (ambient + diffuse + specular);
}

void main()
{
	vec2 gCoords = gl_FragCoord.xy / screenSize;
	vec4 lightColor = pointLight(texture(posTex, gCoords).xyz - worldTransformation, normalize(texture(normalTex, gCoords).xyz));
	vec3 texColor = texture(diffuseTex, gCoords).xyz;
	color = vec4(texColor, 1.0) * lightColor;
}
