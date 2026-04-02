#version 460

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal; 

const float PI=3.1415926535358979323846;

uniform struct LightInfo{
	vec4 Position; //Light position in camera coords
	vec3 L; // Intensity 
} Light[3];

uniform struct MaterialInfo{
	float Rough; //Roughness
	bool Metal; // Metallic(true) or dielectric(false)
	vec3 Color; // diffuse color for dielectrics, f0 for metals
}Material;

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





void main(){
	vec3 sum=vec3(0.0);
	vec3 n=normalize(Normal);
	for(int i=0;i<3;i++){
		sum+=microfacetModel(i,Position,n);
	}

	//gamma 
	sum=pow(sum,vec3(1.0/2.2));
	FragColor=vec4(sum,1);

}
