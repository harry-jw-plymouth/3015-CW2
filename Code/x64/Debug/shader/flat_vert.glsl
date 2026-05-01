#version 460 

layout (location =0) in vec2 VertexPosition;

unifrom mat4 MVP;

void main()
{
    gl_Position=MVP*vec4(VertexPosition,0.0,1.0);
}