#version 460

//general parameters 

layout(location = 0) out vec4 FragColor;
layout (binding=0) uniform sampler2D MainTexture;
in vec2 VTexCoords;

in vec3 Position;
in vec3 Normal; 

//0 is PBR for metals
// 1 is texture
uniform int RenderType;
uniform int RenderMode;

// PBR parameters
const float PI=3.1415926535358979323846;

//pbr structs 
uniform struct LightInfo{
	vec4 Position; //Light position in camera coords
	vec3 L; // Intensity 
} Light[4];

//toon shading levels
const int levels =3;
const float scaleFactor=1.0/levels;

uniform struct MaterialInfo{
	float Rough; //Roughness
	bool Metal; // Metallic(true) or dielectric(false)
	vec3 Color; // diffuse color for dielectrics, f0 for metals
}Material;

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
vec3 pbrToonShade(){ 
	vec3 sum= vec3(0.0);
	vec3 n=normalize(Normal);
	
	
	for(int i=0; i <4;i++){
		vec3 l=vec3(0.0);
		vec3 LightI=Light[i].L;
		if(Light[i].Position.w==0.0){ // directional light
			l=normalize(Light[i].Position.xyz);
		}else{
			l=Light[i].Position.xyz-Position;
			float dist=length(l);
			l=normalize(l);
			LightI/=(dist*dist); //attenuation
		}
		float nDotL = max(dot(n, l), 0.0);
		float ToonValue=ceil(nDotL*levels)*scaleFactor;

		vec3 base;
		if(RenderMode==0){
			base =texture(MainTexture,VTexCoords).rgb;
		}else{
			base = Material.Color;
		}

		if(Material.Metal)
        {
            base =base * 0.6;
        }//else{
		//	base =texture(MainTexture,VTexCoords).rgb;
	//	}
		
		base=base *ToonValue*LightI;
		sum+=base;
	}
    return sum;
}


vec3 shlickFresnelWithTexture(float lDotH){
	vec3 f0=vec3(0.04);
	if(Material.Metal){
		if(RenderMode==0){
			f0=texture(MainTexture,VTexCoords).rgb;
		}
		else{
			f0=Material.Color;
		}
		
	}
	return f0+(1-f0)*pow(1.0-lDotH,5);
}

vec3 microfacetModel(int lightIdx,vec3 position, vec3 n){
	vec3 diffuseBrdf=vec3(0.0); //metallic
	vec3 TextureAlbedo=texture(MainTexture,VTexCoords).rgb;
	if(!Material.Metal){
		if(RenderMode==0){
			diffuseBrdf= TextureAlbedo;
		}
		else{
			diffuseBrdf=Material.Color;
		}
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
	vec3 specBrdf=0.25*ggxDistribution(nDotH)*shlickFresnelWithTexture(lDotH)*geomSmith(nDotL)*geomSmith(nDotV);

	return (diffuseBrdf+PI*specBrdf)*lightI*nDotL;

}



void main(){
	vec3 sum=vec3(0.0);
	vec3 n=normalize(Normal);
	for(int i=0;i<4;i++){
		sum+=microfacetModel(i,Position,n);
	}
	//gamma
	sum=pow(sum,vec3(1.0/2.2));

	vec3 toon=pbrToonShade();

	vec3 Final=mix(sum, toon, 0.5);
	FragColor=vec4(Final,1);

}
