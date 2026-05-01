How does the user interact with your executable? How do you open and control the software
you wrote (exe file)? Yes<br /> 
• How does the program code work? How do the classes and functions fit together and who
does what? yes <br />
• What makes your shader program special and how does it compare to similar things? (Where
did you get the idea from? What did you start with? How did you make yours unique? Did
you start with a given shader/project?) yes<br />
• Include a link to a video report that details the above but also goes into more detail on:<br />
o Are there any software engineering issues, such as the trade-off between performance
and good practice?<br /> yes
o A (brief) evaluation of what you think you have achieved, and what (if anything) you
would do differently, knowing what you now know. Don’t be modest but be realistic! <br />

Add images


# 3015 CW2 Shader tool 
# By Harry Watton

# How to use the software 
## Running the code 
To run the code please navigate to the following directory: Code-> Debug and then open "Project_template.exe". This will run the code. <br />
## Interaction details
 While running the code, WASD can be used to move the camera position to look around the scene and the mouse can be used to change the direction the camera is looking. 
 In addition, there is some basic gameplay that can be interacted with. Around the scene is 5 butterflies, colliding the camera into these butterflies will collect them and make them disappear. When all 5 butterflies have been found, the sword in the center will rise from its stone and glow denoting the games completion

 # How does the program work
## C++
The C++ code runs by first setting up all the values needed for the shaders,mostly in the initscene function. Then the render code is repeatedly run through to redraw it each frame. When each item is drawn, the relative settings/values are set first. For example drawing a sword has edges set to on, disintegration set to on, texture sampling set to off and texture mixing set to off. <br><br>
Each frame, when the draw butterfly function is called, a function to adjust butterflies positioning will be called in addition. This function will bob butterflies up and down as if they are flapping their wings and move them back and forth. This added life and movement to the scene adding a more lively feel <br><br>
This mostly runs uninterrupted until the program is terminated, however there is one thing that changes over time. As mentioned in the section above, the player can collect 5 butterflies to activate the sword. When this is done, a boolean will be set to true, which then will allow a function to be called from the draw sword function, specifically the function 'MoveSwordAfterAllButterfliesFound()'. This will be called each frame and it will slowly move the sword upwards and also increase the intensity of a light glowing from the sword giving the effect of a sword full of power rising from the ground. When it reaches a certain height it will start to bob up and down and the intensity of the light will be stabilised. In addition, when the condition is met, the sword will have disintegration turned off to make it look like it was repaired  <br><br>

In addition a check is done for inputs with both the mouse( for camera direction) and WASD (for movement) being checked.

## Shader code
The code for shaders was made up of 3 parts, the geometry shader which emits vertexes for the edges for silhouette lines. The vertex shader which translates the positioning data into the correct format, and the fragment shader where the majority of shading features were implemented. There were 3 sets of shaders, one for the skybox, which essentially just applied the skybox textures, the combined shaders with geometry shader which were the main shaders used, and finally main shader, which was essentially combined shaders without the geometry shader and silhouette lines. This was to allow a texture to be applied to the floor plane as this would cause errors when passed through the geometry shader due to the fact it couldn't be loaded with adjacency. <br>
The fragment shader in combined shaders has the most interesting functions as well as the most built up code and classes. Main was the base for calling these, with the first check that was done being for whether that vertex was an edge, if it was then black would be returned. This entire section of code would be bypassed if the uniform for setting edge lines was turned off. <br> 
Next, similar was done for disintegration. If the uniform for it was set to off the following would be bypassed. If set to on, the code would check the value of the noise texture and if it was within a certain threshold this vertex would be discarded. This allowed for an easily toggleable damage/ chipped look for the sword.<br> 
Following this was the core shading with PBR and toon shading. Firstly PBR would be run for each light, with the total value from each light added up before applying gamma to keep the light at a reasonable level. To apply the PBR a micro facet model is used which itself uses the ggxDistribution, shlickFresnelWithTexture and geomSmith functions in its calculations. In addition, the texture settings are checked here, if texture sampling is set to off, the set solid colour is used, otherwise the set texture is used. Similar is done for texture mixing, where if set to on, the primary colour/texture is mixed with the secondary texture before applying the microfacet model.<br>
Following this, similar is done but this time with toon shading principles applied, where the light levels are less gradual adding a cartoonish effect. These 2 are then mixed for the final colour. 


# program details 
The shader program stands out from others mainly for its mix of physics based rendering (something generally used for more realistic rendering) with toon shading (something typically used for less realistic style shaders ). This combination allowed for a unique shader look that still ended up looking quite nice aesthetically. Compared to the existing programs it is in a unique place where it has cartoonish elements while still looking like it could be real. The idea came from the idea of shading the scene(the scene being a sword in a stone in a forest) in particular inspiration was taken from the legend of zelda games where a similar scene can be found. In this series there are typically cartoonish looking graphics which was where the idea for this originated. The development of the scene started with PBR as a base before moving onto add the cartoon elements

# video link

# Software engineering issues 
One software engineering issue that came up was to do with the use of models. The models used were not designed with adjacency/triangulation in mind (like the model used in the silhouette lines lab). When using the models I found I identified that it appeared that when the geometry shader added edge quads, it would sometimes mess up when generating these quads. This means that at a few angles there were random black lines floating and when edge lines were active for all models this was very prevalent. For this reason, most models had edge lines turned off for them to ensure that the scene did not become a mess of black lines. In the end, the sword and the stone were the key things with this left on

# Achievements and reflection
Overall I believe I have achieved a well put together shader model with sufficient dynamic options for lots of interesting shading opportunities. By adding the different toggles I believe I have made my shader suitable for lots of different purposes. In addition the scene set up the to showcase it acts as a good tech demo to properly demonstrate the opportunities with the shaders.<br>
While I do think I have done well, there are certainly things that I would change if I was to do this project again. Mainly, I think I would have leaned more heavily into the cartoon styling of the toon shading and leaned away from PBR. While I do like how the final result came out, I do think there were better options for applying this. For example shadow mapping would have added some nice realistic depth while keeping a cool cartoon style 

