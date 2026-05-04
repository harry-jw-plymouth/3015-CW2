#version 460

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location = 2)in vec2 VertexTextureCoords;

out vec3 Normal;
out vec2 VTexCoords;

out vec3 Position;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main(){
    Normal=normalize(NormalMatrix*VertexNormal);
    Position=(ModelViewMatrix*vec4(VertexPosition,1.0)).xyz;
    gl_Position =MVP*vec4 (VertexPosition,1.0);
    VTexCoords=VertexTextureCoords;
}