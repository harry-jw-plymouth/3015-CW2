#version 460

struct LightInfo{
    vec4 Position;
    vec3 Intensity;
    vec3 La;
    vec3 L;
};
uniform LightInfo Light;

struct MaterialInfo{
    vec3 Ka; //ambient reflectivity 
    vec3 Kd;  //diffuse reflectivity
    vec3 Ks; // specular reflectivity
    float shininess;  //specular shininess factor
};
uniform MaterialInfo Material;

uniform vec4 LineColor;

in vec3 Gposition;
in vec3 GNormal;

flat in int GIsEdge;
layout(location=0) out vec4 FragColor;

//toon shading levels
const int levels =3;
const float scaleFactor=1.0/levels;

vec3 toonShade(){
    vec3 s=normalize(Light.Position.xyz-Gposition.xyz);
    vec3 ambient=Material.Ka;
    float cosine=dot(s,GNormal);
    vec3 diffuse=Material.Kd*ceil(cosine*levels)*scaleFactor;

    return Light.Intensity*(ambient+diffuse);
}

void main(){
    if(GIsEdge==1){
        FragColor=LineColor;
    }else{
        FragColor=vec4(toonShade(),1.0);
    }
}