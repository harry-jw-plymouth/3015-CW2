#version 460

layout (binding=0) uniform sampler2D MainTexture;
in vec2 GTexCoord;

// PBR parameters
const float PI=3.1415926535358979323846;

//pbr structs
uniform struct PBRLightInfo{
	vec4 Position; //Light position in camera coords
	vec3 L; // Intensity 
} PBRLight[3];

uniform struct PBRMaterialInfo{
	float Rough; //Roughness
	bool Metal; // Metallic(true) or dielectric(false)
	vec3 Color; // diffuse color for dielectrics, f0 for metals
	vec3 Ka; 
	vec3 Kd; 
	vec3 Ks; 
	float shininess; 
}PBRMaterial;


uniform vec4 LineColor;

in vec3 Gposition;
in vec3 GNormal;

flat in int GIsEdge;
uniform int RenderMode; 
//0 is textured, 1 is untextured

uniform int EdgeOn;
layout(location=0) out vec4 FragColor;

//toon shading levels
const int levels =3;
const float scaleFactor=1.0/levels;

vec3 pbrToonShade(){ 
	vec3 sum= vec3(0.0);
	vec3 n=normalize(GNormal);
	
	
	for(int i=0; i <3;i++){
		vec3 l=vec3(0.0);
		vec3 LightI=PBRLight[i].L;
		if(PBRLight[i].Position.w==0.0){ // directional light
			l=normalize(PBRLight[i].Position.xyz);
		}else{
			l=PBRLight[i].Position.xyz-Gposition;
			float dist=length(l);
			l=normalize(l);
			LightI/=(dist*dist); //attenuation
		}
		float nDotL = max(dot(n, l), 0.0);
		float ToonValue=ceil(nDotL*levels)*scaleFactor;

		vec3 base;
		if(RenderMode==0){
			base =texture(MainTexture,GTexCoord).rgb;
		}else{
			base = PBRMaterial.Color;
		}

		if(PBRMaterial.Metal)
        {
            base =base * 0.6;
        }//else{
		//	base =texture(MainTexture,GTexCoord).rgb;
	//	}
		
		base=base *ToonValue*LightI;
		sum+=base;
	}
    return sum;
}
//pbr functions
float ggxDistribution(float nDotH){ 
	float alpha2=PBRMaterial.Rough*PBRMaterial.Rough*PBRMaterial.Rough*PBRMaterial.Rough;
	float d=(nDotH*nDotH)*(alpha2-1) +1;
	return alpha2/(PI*d*d);
}
float geomSmith(float dotProd){
	float k=(PBRMaterial.Rough+1.0)*(PBRMaterial.Rough+1.0)/8.0;
	float denom=dotProd*(1.0-k)+k;
	return 1.0/denom;
}
vec3 shlickFresnelWithTexture(float lDotH){
	vec3 f0=vec3(0.04);
	if(PBRMaterial.Metal){
		if(RenderMode==0){
			f0=texture(MainTexture,GTexCoord).rgb;
		}
		else{
			f0=PBRMaterial.Color;
		}
		
	}
	return f0+(1-f0)*pow(1.0-lDotH,5);
}
vec3 microfacetModel(int lightIdx,vec3 position, vec3 n){
	vec3 diffuseBrdf=vec3(0.0); //metallic
	vec3 TextureAlbedo=texture(MainTexture,GTexCoord).rgb;
	if(!PBRMaterial.Metal){
		if(RenderMode==0){
			diffuseBrdf= TextureAlbedo;
		}
		else{
			diffuseBrdf=PBRMaterial.Color;
		}
	}

	vec3 l=vec3(0.0),lightI=PBRLight[lightIdx].L;
	if(PBRLight[lightIdx].Position.w==0.0){ // directional light
		l=normalize(PBRLight[lightIdx].Position.xyz);
	}else{
		l=PBRLight[lightIdx].Position.xyz-position;
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
	vec3 specBrdf=0.25*ggxDistribution(nDotH)*shlickFresnelWithTexture(lDotH)*geomSmith(nDotL)*geomSmith(nDotV);

	return (diffuseBrdf+PI*specBrdf)*lightI*nDotL;

}


void main(){
	if(GIsEdge==1){
		if(EdgeOn==1){
			FragColor=LineColor;
		}else{
			discard;
		}
		return;
	}
	vec3 sum=vec3(0.0);
	vec3 n=normalize(GNormal);
	for(int i=0;i<3;i++){
		sum+=microfacetModel(i,Gposition,n);
	}
	//gamma
	sum=pow(sum,vec3(1.0/2.2));

	vec3 toon=pbrToonShade();

	vec3 Final=mix(sum, toon, 0.5);
	FragColor=vec4(Final,1);
}