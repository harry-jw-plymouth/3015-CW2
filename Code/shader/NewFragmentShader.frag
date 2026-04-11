#version 460

//general parameters 

layout(location = 0) out vec4 FragColor;

//in vec3 Position;
//in vec3 Normal; 

//0 is PBR for metals
// 1 is texture
uniform int RenderType;

// PBR parameters
const float PI=3.1415926535358979323846;

//pbr structs 
uniform struct LightInfo{
	vec4 Position; //Light position in camera coords
	vec3 L; // Intensity 
} Light[3];



uniform struct MaterialInfo{
	float Rough; //Roughness
	bool Metal; // Metallic(true) or dielectric(false)
	vec3 Color; // diffuse color for dielectrics, f0 for metals
}Material;

//silhouette line parameters

in vec3 Gposition;
in vec3 GNormal;


uniform vec4 LineColor;
flat in int GIsEdge;

//toon shading levels
const int levels =3;
const float scaleFactor=1.0/levels;

//silhouette lines structs
struct TexMaterialInfo{
    vec3 Ka; //ambient reflectivity 
    vec3 Kd;  //diffuse reflectivity
    vec3 Ks; // specular reflectivity
    float shininess;  //specular shininess factor
};
uniform TexMaterialInfo TexMaterial;

struct TexLightInfo{
    vec4 Position;
    vec3 Intensity;
    vec3 La;
    vec3 L;
};
uniform TexLightInfo TexLight;







//PBR functions

float ggxDistribution(float nDotH){ 
	float alpha2=Material.Rough*Material.Rough*Material.Rough*Material.Rough;
	float d=(nDotH*nDotH)*(alpha2-1) +1;
	return alpha2/(PI*d*d);
}
float geomSmith(float dotProd){
	float k=(Material.Rough+1.0)*(Material.Rough+1.0)/8.0;
	float denom=dotProd*(1.0-k)+k;
	return 1.0/denom;
}

vec3 shlickFresnel(float lDotH){
	vec3 f0=vec3(0.04);
	if(Material.Metal){
		f0=Material.Color;
	}
	return f0+(1-f0)*pow(1.0-lDotH,5);
}

vec3 microfacetModel(int lightIdx,vec3 position, vec3 n){
	vec3 diffuseBrdf=vec3(0.0); //metallic
	if(!Material.Metal){
		diffuseBrdf=Material.Color;
	}

	vec3 l=vec3(0.0),lightI=Light[lightIdx].L;
	if(Light[lightIdx].Position.w==0.0){ // directional light
		l=normalize(Light[lightIdx].Position.xyz);
	}else{
		l=Light[lightIdx].Position.xyz-position;
		float dist=length(l);
		l=normalize(l);
		lightI/=(dist*dist); //attenuation
	}
	vec3 v=normalize(-position);
	vec3 h=normalize(l+v);
	float nDotH=dot(n,h);
	float lDotH=dot(l,h);
	float nDotL=max(dot(n,l),0.0);
	float nDotV=dot(n,v);
	vec3 specBrdf=0.25*ggxDistribution(nDotH)*shlickFresnel(lDotH)*geomSmith(nDotL)*geomSmith(nDotV);

	return (diffuseBrdf+PI*specBrdf)*lightI*nDotL;

}

// Sillhouette line functions

vec3 toonShade(){
    vec3 s=normalize(TexLight.Position.xyz-Gposition.xyz);
    vec3 ambient=TexMaterial.Ka;
    float cosine=dot(s,GNormal);
    vec3 diffuse=TexMaterial.Kd*ceil(cosine*levels)*scaleFactor;

    return TexLight.Intensity*(ambient+diffuse);
}

void main(){
	if(RenderType==1){
		//texture rendering
		//FragColor=vec4(0.0);
		if(GIsEdge==1){
			FragColor=LineColor;
		}else{
			FragColor=vec4(toonShade(),1.0);
		}

	}
	else{
		// PBR 
		vec3 sum=vec3(0.0);
		vec3 n=normalize(GNormal);
		for(int i=0;i<3;i++){
			sum+=microfacetModel(i,Gposition,n);
		}

		//gamma 
		sum=pow(sum,vec3(1.0/2.2));
		FragColor=vec4(sum,1);
	}

	

}
