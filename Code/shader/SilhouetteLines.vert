#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location =1)in vec3 VertexNormal;
layout (location = 2)in vec2 VertexTextureCoords;

out vec3 VNormal;
out vec3 Vposition;
out vec2 VTexCoords;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
    VNormal=normalize(NormalMatrix*VertexNormal);
    Vposition=vec3(ModelViewMatrix*vec4(VertexPosition,1.0));
    VTexCoords=VertexTextureCoords;
    gl_Position=MVP*vec4(VertexPosition,1.0);
}
