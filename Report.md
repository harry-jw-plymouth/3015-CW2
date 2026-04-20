-Which version Visual Studio and Operating System you used to test/write the code
locally? Done <br />
o How does it work? <br />
o What makes your shader program special and how does it compare to similar things? 
(Where did you get the idea from? What did you start with? How did you make yours
unique? Did you start with a given shader/project?)<br /> 
o Anything else which will help us to understand how your prototype works.<br />
o A link to the GitHub repo you created for public use done<br />
o A link to the unlisted YouTube Video<br />

# Comp 3015 CW2 Optimised Developer tool 
# By Harry Watton 

# Versioning information 
The code was tested with the following specifications <br />
Operating system: windows 11<br />
Visual studio version: Visual studio 2022<br />

# How does it work?
A simple description of how my shader program works is that it uses a geometery shader to allow for silhouette lines before the fragment shader applies phyics based rendering(PBR) and  applying toon shading to add the cartoon style look that cones with silhouette lines. Below is details on the overall pipeline as well as details on the individual features that come together to build said pipeline 
## Pipeline 
This section details the shader pipeline and the process it follows to provide shading 
### Vertex shader 
The vertex shader is the first step of the shader pipeline and it lays the essential ground work for the rest of the shaders.<br /> The key functionality done here is that it converts the vertex positions of the object mesh into a format usable by the GPU, without it the rendering would not be able to occur. The transformation is completed using the model view projection by using the model view position data and then using projection to make the object appear smaller/bigger depending on how close or distant from the camera the object is, <br />
The following stages rely on the vertex shader as, the position and Normal data transformed and set up here are used in the geometery shader for detecting whether something is an edge for silhouette lines and used in fragment shader for lighting calulcuations.
### geometery shader 

### Fragment shader 


The shader pipeline
## Individual techniques 
This section details the individual technique that come together to make up the shaders
## Physics based rendering (PBR)
## Silhouette lines and toon shading



# What makes your shader program special and how does it compare to similar things? 
My shader program is special in the way it combines PBR and cell shading/silhouette lines 

#Anything else which will help us to understand how your prototype works 

## Asset usage 

# Github reopsitory link
https://github.com/harry-jw-plymouth/3015-CW2.git

# A link to the unlisted YouTube Video




sword 
https://free3d.com/3d-model/sword-medieval-29559.html 
