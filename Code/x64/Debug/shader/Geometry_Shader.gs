#version 460

//input primatives
layout(triangles_adjacency ) in;
//layout(triangles ) in;
layout( triangle_strip, max_vertices = 15)out;

//output primatives
out vec3 GNormal; //normal used by fragment shader 
out vec3 Gposition; //positon used for fragment shader 
out vec2 GTexCoord;
out vec3 GWorldPos;

//which triangle edges are silhouette edges
//0 not edge, 1 is an edge
flat out int GIsEdge;

in vec3 VNormal[];
in vec3 Vposition[];
in vec2 VTexCoords[];
in vec3 VWorldPos[];

uniform float EdgeWidth;
uniform float PctExtend;

bool isFrontFacing(vec3 a,vec3 b,vec3 c){
    return ((a.x*b.y-b.x* a.y)+(b.x*c.y-c.x*b.y)+(c.x*a.y-a.x*c.y))>0;
}
void emitEdgeQuad(vec3 e0,vec3 e1){
    vec2 ext=PctExtend* (e1.xy-e0.xy);
    vec2 v=normalize(e1.xy-e0.xy);
    vec2 n= vec2(-v.y,v.x)*EdgeWidth;

    GIsEdge=1; //part of the sil. edge

    gl_Position=vec4(e0.xy-ext,e0.z,1.0);
    GTexCoord = vec2(0.0);
    EmitVertex();

    gl_Position=vec4(e0.xy-n,e0.z,1.0);
    GTexCoord = vec2(0.0);
    EmitVertex();

    gl_Position=vec4(e1.xy+ext,e1.z,1.0);
    GTexCoord = vec2(0.0);
    EmitVertex();

    gl_Position=vec4(e1.xy-n+ext,e1.z,1.0);
    GTexCoord = vec2(0.0);
    EmitVertex();


    EndPrimitive();

}

void main(){
    vec3 p0= gl_in[0].gl_Position.xyz/gl_in[0].gl_Position.w;
    vec3 p1= gl_in[1].gl_Position.xyz/gl_in[1].gl_Position.w;
    vec3 p2= gl_in[2].gl_Position.xyz/gl_in[2].gl_Position.w;
    vec3 p3= gl_in[3].gl_Position.xyz/gl_in[3].gl_Position.w;
    vec3 p4= gl_in[4].gl_Position.xyz/gl_in[4].gl_Position.w;
    vec3 p5= gl_in[5].gl_Position.xyz/gl_in[5].gl_Position.w;

    if(isFrontFacing(p0,p2,p4)){
        if(!isFrontFacing(p0,p1,p2))
        {
            emitEdgeQuad(p0,p2);
        }
        if(!isFrontFacing(p2,p3,p4))
        {
            emitEdgeQuad(p2,p4);
        }
        if(!isFrontFacing(p4,p5,p0))
        {
            emitEdgeQuad(p4,p0);
        }
        
    }

    //output the original triangle

    GIsEdge=0; //this triangle is not part of an edge

    GNormal=VNormal[0];
    Gposition=Vposition[0];
    GWorldPos=VWorldPos[0];
    GTexCoord=VTexCoords[0];
    gl_Position=gl_in[0].gl_Position;
    EmitVertex();

    GNormal=VNormal[2];
    Gposition=Vposition[2];
    GWorldPos=VWorldPos[2];
    GTexCoord=VTexCoords[2];
    gl_Position=gl_in[2].gl_Position;
    EmitVertex();
    
    GNormal=VNormal[4];
    Gposition=Vposition[4];
    GWorldPos=VWorldPos[4];
    GTexCoord=VTexCoords[4];
    gl_Position=gl_in[4].gl_Position;
    EmitVertex();

    EndPrimitive();
    //finalise primative and send it along the pipeline 
}