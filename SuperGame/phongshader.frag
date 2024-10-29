uniform sampler2D texture01;

uniform vec4 materialAmbient;
uniform vec4 materialDiffuse;
uniform vec4 materialSpecular;
uniform vec4 materialEmissive;
uniform float specularPower;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

varying vec2 outUv;
varying vec3 eyespacePosition;
varying vec3 eyespaceNormal;

void main(void) {
	vec3 normal = normalize(eyespaceNormal);
	float diffuseFactor = dot(normal, -lightDirection);
	vec4 diffuseColor = texture2D(texture01, outUv) * materialDiffuse * diffuseFactor;

	if (specularPower > 0.9) 
	{
		vec3 surfaceToCamera = normalize(cameraPosition - eyespacePosition);
		float specularFactor = dot(surfaceToCamera, reflect(lightDirection, normal));
		specularFactor = pow(max(0.0, specularFactor), specularPower);
		vec4 specularColor = materialSpecular * specularFactor;

		gl_FragColor = materialAmbient + diffuseColor + materialEmissive + specularColor;
	} 
	else 
	{
		gl_FragColor = materialAmbient + diffuseColor + materialEmissive;
	}
}