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
To put it simply, my shader program runs by generating edge quads using a geometry shader, then in the fragment shader, PBR and toon shading is applied. In addition there is the option to toggle between flat colours and full textures, displaying edge lines, and disintegration effects to add a weathered look to the objects as well as texture mixing.


## Pipeline 
This section details the shader pipeline and the process it follows to provide shading 
### Vertex shader 
The vertex shader is the first step of the shader pipeline and it lays the essential ground work for the rest of the shaders.<br /> The key functionality done here is that it converts the vertex positions of the object mesh into a format usable by the GPU, without it the rendering would not be able to occur. The transformation is completed using the model view projection by using the model view position data and then using projection to make the object appear smaller/bigger depending on how close or distant from the camera the object is, <br />
The following stages rely on the vertex shader as the position and Normal data transformed and set up here are used in the geometry shader for detecting whether something is an edge for silhouette lines and used in fragment shader for lighting calculations.
### geometry shader 
The next step in the shader pipeline was the geometry shader <br />
Using the MVP matrix seen in vertex shader, whole triangles potentially see transformations. In most cases the vertices do not see transformations, but in some cases a triangle may be determined to be an edge, in which case extra geometry is generated to act as the edge which is also flagged to determine it is an edge. This is then passed to the fragment shader. <br /> 
The process of this is included to allow for silhouette lines to be included, for more details please see the relevant section below
### Fragment shader 
The final step is the fragment shader, where all the data set up previously can finally be turned into the output colour of the shader <br />
The first step fragment shader takes is to check if the geometry it is providing colour for is an edge. If it is then the shader will simply return the colour black. <br /> 
Otherwise, the geometry will have PBR applied and then get overlayed by a toon shading method to create the final colour which is then returned and displayed on the object.  <br />
For more details on the toon shading, silhouette lines and PBR please see the section below detailing the individual techniques 

## Individual techniques 
This section details the individual techniques that come together to make up the shaders
## Physics based rendering (PBR)
PBR was used as the base of the project,and was one of the main shading features.
It was chosen over other lighting models (e.g Blinn Phong) due to the extra detail and accuracy it allows <br /> 
In my code, PBR exists as part of the fragment shader, where normals and position data is already set up by the vertex shader. The first step the PBR code takes is normalising the Normal value followed by applying the microfacet model. <br /> 
In the process of applying the micro facet model, the first step is checking if the material is a metal which is done using the value in the material struct. If it is not a metal, the diffuse colour is set to the colour defined in the struct. This is important as it ensure Dielectrics do not end up too shiny.The next step is determining the lights direction where the the intensity of the light is increased based on the distance from the light, this is what is known as attenuation and it is important as otherwise the mesh would be equally bright regardless of its position in relation to the light. The next step sees the position vector and halfway vectors normalised to ensure correct calculations. This is then followed by a set of dot product calculations which are then used to calculate the specular value. This is a set of values multiplied together to make up the final value which makes use of ggxDistribution (controls how rough the surface looks by adding some variance to highlights), shlickFresnel(determines how reflective the material is relative to the angle of the light) and geomsmith (controls what part of the mesh is blocked) calculations <br /> 
The microfacet model was applied multiple times,once for every light. The value of each application was totalled up and then used for gamma correction. This was the final step needed before it could be mixed with toon shading and then returned as the frag colour <br /> 

Please note, the code from lab 10 was used as a base and adapted for this project 
## Silhouette lines and toon shading
Another key technique used was silhouette lines with toon shading. This was used to give a cartoon look which makes the shading a lot more stylised and unique, and it created an interesting contrast by being combined with the more realistic element of the physic based rendering. <br /> 
<br />
The inclusion of this technique prompted the inclusion of the geometry shader as well as the need for each mesh to be loaded with adjacency to allow for the desired geometry shading(done using the built-in function in objmesh.cpp). In the geometry shader, a set of vertices (e.g a triangle) is looked at. When looking at it, it will determine if the vertices are an edge of a mesh and if it is then it is marked as an edge and extra geometry is added to be used as the silhouette line later. These faces are then passed to the fragment shader. Before being passed, a flag is set to determine if the vertices are an edge  <br />
In the fragment shader the edge flag is checked. If it is indeed an edge, the fragment shader will return flat black as the colour. meaning edges are shaded with a clear differentiation from the more regular shaded sections <br /> <br />
If it is not an edge, then the PBR code is run through (details of this can be seen in the section above), but once that is complete, a toon based shader adapted to work with PBR is run. This runs through similar steps as the PBR code but has a toon overlay, where instead of a smooth  light transition, shadows/light levels change more suddenly, this leads to more simple shading like what is seen in cartoons.<br />
This is then mixed with the PBR calculation seen earlier to create the mixed look the shader aims for . In addition, the mixing can be controlled with one shading technique given bias to have more of a focus compared to the other if it makes sense, this is controlled by the value in the line where the 2 are mixed with 0.5 being balanced and increasing or decreasing the value (it must stay between 0 and 1) increasing the prevalence of one of the techniques. <br /><br />
The edge lines can be controlled and adjusted with 2 variables. PctExtend increases the length of the silhouette, allowing for control of how silhouettes overlap. Edgewidth determines how wide the edges actually are, allowing for control over how thick/thin the lines should be and also giving the option between more bold lines or more subtle lines depending on the situation.<br />


Please note,the code for this technique was an adapted version of the code seen in lab 6 part 3, the geometry shader is essentially 1:1 with said code. 

# What makes your shader program special and how does it compare to similar things? 
My shader program is special in the way it combines PBR and cell shading/silhouette lines 

# Anything else which will help us to understand how your prototype works 
## Gameplay description 
Some very basic gameplay was added to help the scene come alive. In this gameplay, there is a set of 5 butterflies moving around the scene. When the player collides with a butterfly, that butterfly is counted as collected and will disappear. When all 5 are collected, the game will be seen as completed, and as a result of the game being completed the sword will rise from the stone and begin to glow. It is very simple gameplay but it acts as a method of giving the player a task to complete

## Code origins
The code used in the program was taken mostly from lab sessions and then combined and adjusted to fit the purposes of this project.<br>
The starting point of the code was to use the PBR from lab session 10 and test out how the metallics looked on the sword model. After particularly liking how this looked,I decided to use this as my base. Originally the plan was to build off my scene from CW1 but after seeing this I decided to completely remake it and where necessary pull things over.<br>
With this code base created, the next steps were implementing the silhouette lines and and toon shading code from lab session 8,prompting the use of the geometry shader.  This ultimately caused the creation of a second set of shaders, as the plane used for the ground of the scene was not compatible with the adjacency triangles required. These shaders are essentially the same, just without the addition of silhouette lines. Finally the disintegration effect from lab session 9 was implemented to allow for a damaged look on the sword. This required some adjustment to the geometry shader to pass more values. <br>
As previously mentioned, aspects of my submission for CW1 were re implemented in some ways. For example the same tree model was used and the code for moving butterflies was partially reused but adapted to support an array of butterfly position values

## Asset usage 
A variety of assets were used in the development of this project. These are detailed below as well as their source.<br />
### Models
Sword model: https://free3d.com/3d-model/medieval-sword-69788.html <br />
Rock model and texture (reused from 3016 cw2): https://www.turbosquid.com/3d-models/rock07base3ds-3d-1899446 <br />
Tree model (reused from 3016 cw2): https://www.turbosquid.com/3d-models/gentree-103-generic-tree-103-3d-model-2062798 <br />
Butterfly model and texture (reused from 3016 cw2): https://www.turbosquid.com/3d-models/butterfly-fly-3d-obj/460590
### Textures
Skybox (reused from 3015 cw1): https://opengameart.org/content/forest-skyboxes <br>
moss/leaves: Source code from labs <br>
Ground: https://www.magnific.com/free-photo/photo-ground-texture-pattern_198163212.htm#fromView=keyword&page=1&position=4&uuid=bea7c744-c5d3-4edb-8539-c106a228312c&query=Forest+ground+texture+cartoon    <br> 
Bark: https://www.magnific.com/free-vector/dark-wood-texture_1036321.htm#fromView=keyword&page=1&position=20&uuid=40d5d19b-9995-44c6-bb17-8d353f1b190d&query=Bark+texture  <br>

 # Use of AI statement


# Github repository link
https://github.com/harry-jw-plymouth/3015-CW2.git <br />


# A link to the unlisted YouTube Video




sword 
https://free3d.com/3d-model/sword-medieval-29559.html 

