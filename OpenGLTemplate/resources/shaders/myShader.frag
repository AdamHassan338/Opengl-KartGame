#version 400 core

in vec4 colour;
in vec2 texCoord;
in vec3 normal;
//in vec3 FragPos;

out vec4 fragColour;

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube cubeMapTex;
in vec3 reflected;

vec3 Lighting;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct SpotLight
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float linear;
	float constant;
	float cutoff;
	float attenuationExp;
};

struct PointLight
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform PointLight worldLight; 
uniform SpotLight carLight; 
uniform MaterialInfo material1;


in vec4 p;
in vec3 n;



vec3 BlinnPhongSpotlightModel(SpotLight light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = light.La * material1.Ma;
	float Distance = length(light.position - p);
	float attenuationfactor =light.constant + (light.linear * Distance)  + (light.attenuationExp * Distance * Distance);
	
	
	if (angle < cutoff) {
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material1.Md * sDotN;
		vec3 specular = vec3(0.0);
		if (sDotN > 0.0)
			specular = light.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
		return (ambient + spotFactor * (diffuse + specular)* (1/attenuationfactor)) ;
	} else 
		return ambient /attenuationfactor;
}



vec3 BlinnPhongPointlightModel(PointLight light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position - p));
	vec3 ambient = light.La * material1.Ma;
	vec3 v = normalize(-p.xyz);
	vec3 h = normalize(v + s);
	float sDotN = max(dot(s, n), 0.0);
	vec3 diffuse = light.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0);
	if (sDotN > 0.0)
		specular = light.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
	return ambient + (diffuse + specular);

}


vec3 PhongModel(vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(worldLight.position - p));
	vec3 v = normalize(-p.xyz);
	vec3 r = reflect(-s, n);
	vec3 h = normalize(v + s);
	vec3 ambient = worldLight.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0);
	vec3 diffuse = worldLight.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0);
	if (sDotN > 0.0)
		specular = worldLight.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
	
	return ambient + diffuse + specular;

}


void main(){

	vec4 vTexColour = texture(sampler0, texCoord);
	Lighting = BlinnPhongPointlightModel(worldLight,p,n)  +  BlinnPhongSpotlightModel(carLight,p,n);
	fragColour = vTexColour * vec4(Lighting, 1.0f);
	//fragColour = texture(cubeMapTex, normalize(reflected));
}