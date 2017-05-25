#version 130
in vec4 s_vPosition;
in vec2 texCoord;
uniform mat4 transform;
uniform vec3 obj_position;
uniform vec4 rotation; /* x,y,z,angle */
uniform vec3 cameraPosition;
varying vec2 tCoord;
varying vec4 pos;
varying float distPosToCam;
mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}
void main() {
	vec4 vert = s_vPosition;
	vec4 finalvert = vert + vec4( obj_position, 1.0);
	tCoord = texCoord;
	pos = finalvert;
	vec4 finalPosition =  transform * finalvert;
    distPosToCam = length(finalPosition);
    gl_Position = finalPosition;
}	