attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;

uniform mat4 mWorldViewProj;
uniform mat4 mInvWorld;
uniform mat4 mTransWorld;
uniform vec3 mLightPos;
uniform vec4 mLightColor;

varying mediump vec4 v_color;
varying mediump vec2 v_texCoord;

void main(void)
{
	gl_Position = mWorldViewProj * vec4(inVertexPosition,1.0);
	
	vec4 normal = vec4(inVertexNormal, 0.0);
	normal = mInvWorld * normal;
	normal = normalize(normal);
	
	vec4 worldpos = vec4(inVertexPosition,1.0) * mTransWorld;
	
	vec4 lightVector = worldpos - vec4(mLightPos,1.0);
	lightVector = normalize(lightVector);
	
	float tmp2 = dot(-lightVector, normal);
	
	vec4 tmp = mLightColor * tmp2;
	v_color = vec4(tmp.x, tmp.y, tmp.z, 0.0);
	
	v_texCoord = inTexCoord0;
}
