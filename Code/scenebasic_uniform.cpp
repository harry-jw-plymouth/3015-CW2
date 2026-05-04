#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <sstream>



SceneBasic_Uniform::SceneBasic_Uniform() :plane(20,20,1,1),
tPrev(0.0f),lightPos(5.0f,5.0f,5.0f,1.0f){
    SwordMesh = ObjMesh::loadWithAdjacency("media/Sword.obj");
    RockMesh = ObjMesh::loadWithAdjacency("media/Rock07-Base.obj");
	TreeMesh = ObjMesh::loadWithAdjacency("media/Tree.obj");
	ButterflyMesh = ObjMesh::loadWithAdjacency("media/Butterfly/_butterfly.obj");
}
void SceneBasic_Uniform::LoadTextures() {
    RockTexture= Texture::loadTexture("media/texture/rock/Rock07-Base-Diffuse.png");
    TreeTexture = Texture::loadTexture("media/texture/Bark.png");
    ForestFloorTexture=Texture::loadTexture("media/texture/ForestFloor.png");
    ButterflyTexture = Texture::loadTexture("media/Butterfly/texture.bmp");
    MossTexture = Texture::loadTexture("media/texture/moss.png");
}
void SceneBasic_Uniform::Mouse_CallBack(double Xpos, double Ypos) {
     //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        cameraLastXPos = (float)Xpos;
        cameraLastYPos = (float)Ypos;
        mouseFirstEntry = false;
    }  //Sets values for change in position since last frame to current frame
    float xOffset = (float)Xpos - cameraLastXPos;
    float yOffset = cameraLastYPos - (float)Ypos;

    //Sets last positions to current positions for next frame
    cameraLastXPos = (float)Xpos;
    cameraLastYPos = (float)Ypos;

    //Moderates the change in position based on sensitivity value
    const float sensitivity = 0.05f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Adjusts yaw & pitch values against changes in positions
    cameraYaw += xOffset;
    cameraPitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (cameraPitch > 89.0f)
    {
        cameraPitch = 89.0f;
    }
    else if (cameraPitch < -89.0f)
    {
        cameraPitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    CameraFront = normalize(direction);
}
void SceneBasic_Uniform::ProcessUserInput(int key, int action) {
    const float movementSpeed = 2.0f * deltaTime;

    if (action == GLFW_PRESS) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (key == GLFW_KEY_W) {
                EyeCoordinates += movementSpeed * CameraFront;
            }
            else if (key == GLFW_KEY_A) {
                EyeCoordinates -= normalize(cross(CameraFront, CameraUp)) * movementSpeed;
            }
            else if (key == GLFW_KEY_S) {
                EyeCoordinates -= movementSpeed * CameraFront;
            }
            else if (key == GLFW_KEY_D) {
                EyeCoordinates += normalize(cross(CameraFront, CameraUp)) * movementSpeed;
            }
        }
    }
}
void SceneBasic_Uniform::SetupSkybox() {
    SkyBoxShaders.use();

    glEnable(GL_DEPTH_TEST);

    projection = mat4(1.0f);
    model = mat4(1.0f);
    float angle = radians(90.0f);
    GLuint SkyBoxTexture = Texture::loadCubeMap("media/texture/ForestSkyBox/Forest");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBoxTexture);

    GroundShaders.use();
    
}
void SceneBasic_Uniform::initScene()
{
    compile(); 

    LoadTextures();

    SetupSkybox();

    SetUpNoise();

    GroundShaders.use();
    GroundShaders.setUniform("RenderType", 0);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(
        vec3(0.0f, 4.0f, 7.0f),
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)
    );
	projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.5f, 100.0f);

    lightAngle = 0.0f;
    lightRotationSpeed = 1.5f;

    //Setup lights for ground shaders
    GroundShaders.setUniform("Light[0].L", vec3(45.0f));
	GroundShaders.setUniform("Light[0].Position", view * lightPos);
    GroundShaders.setUniform("Light[1].L", vec3(0.3f));
    GroundShaders.setUniform("Light[1].Position", vec4(0,0.15f,-1.0f,0));
    GroundShaders.setUniform("Light[2].L", vec3(45.0f));
    GroundShaders.setUniform("Light[2].Position", view * glm::vec4(-7, 3, 7, 1));
    GroundShaders.setUniform("Light[3].L", vec3(SwordLightIntensity));
    GroundShaders.setUniform("Light[3].Position", view * glm::vec4(SwordPos, 1.0f));

    //Setup lights for main shaders
    MainShaders.use();
    MainShaders.setUniform("PBRLight[0].L", vec3(45.0f));
    MainShaders.setUniform("PBRLight[0].Position", view * lightPos);

    MainShaders.setUniform("PBRLight[1].L", vec3(0.3f));
    MainShaders.setUniform("PBRLight[1].Position", vec4(0, 0.15f, -1.0f, 0));

    MainShaders.setUniform("PBRLight[2].L", vec3(45.0f));
    MainShaders.setUniform("PBRLight[2].Position", view * glm::vec4(-7, 3, 7, 1));

    //set sword light to not glow yet
    // will be activated when butterflies found
    MainShaders.setUniform("PBRLight[3].L", vec3(0.0f));
    MainShaders.setUniform("PBRLight[3].Position", view * glm::vec4(SwordPos+vec3(0.0f,0.5f,0.0f), 1.0f));

    MainShaders.setUniform("PBRMaterial.Kd", 0.7f, 0.5f, 0.2f);
    MainShaders.setUniform("PBRMaterial.Ka", 0.2f, 0.2f, 0.2f);

    //silhouette lines set up 
    MainShaders.setUniform("EdgeWidth", 0.009f);
    MainShaders.setUniform("PctExtend", 0.03f);
    MainShaders.setUniform("LineColor", vec4(0.05f, 0.0f, 0.05f, 1.0f));
   
    GroundShaders.use();
}
void SceneBasic_Uniform::compile()
{
    try {
        MainShaders.compileShader("shader/MainVertexShader.vert");
        MainShaders.compileShader("shader/MainFragmentShader.frag");
        MainShaders.compileShader("shader/Geometry_Shader.gs");
        MainShaders.link();
        

        GroundShaders.compileShader("shader/GroundVertexShader.vert");
        GroundShaders.compileShader("shader/GroundFragmentShader.frag");
        GroundShaders.link();
        GroundShaders.use();

        SkyBoxShaders.compileShader("shader/SkyBoxVertexShader.vert");
        SkyBoxShaders.compileShader("shader/SkyBoxFragmentShader.frag");
        SkyBoxShaders.link();

    }
    catch (GLSLProgramException &e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
void SceneBasic_Uniform::update(float t)
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) {
        deltaT = 0.0f;
    }
    deltaTime = deltaT;
    //update view for updated eye coorindates
    view = glm::lookAt(EyeCoordinates, EyeCoordinates + CameraFront, CameraUp);

    angle += 0.25f * deltaT;
    if (angle > glm::two_pi<float>())angle -= glm::two_pi<float>();

    tPrev = t;
    if (animating()) {
        lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>());
		lightPos.x = glm::cos(lightAngle) * 7.0f;
        lightPos.y = 3.0f;
		lightPos.z = glm::sin(lightAngle) * 7.0f;

    }
	CheckForButterflyCollisions();
}
void SceneBasic_Uniform::CheckForButterflyCollisions() {
    // for each butterfly, check if current position is within a certain distance and if so declare if found
    for (int i = 0; i < 5; i++) {
        if (!Butterfliesfound[i]) {
            float distance = length(EyeCoordinates - (ButterflyPositions[i] + ButterflyModifiedPosition[i]));
            if (distance < 1.0f) {
                Butterfliesfound[i] = true;
                std::cout << "Butterfly " << i + 1 << " found!" << std::endl;
                CheckIfAllButerfliesFound();
            }
        }
    }  
}
void SceneBasic_Uniform::CheckIfAllButerfliesFound() {
    ButterfliesAllFound = true;
    //loop  through all butterflies, if all found set value to true so sword knows to update
    for (int i = 0; i < NumberOfButterFlies; i++) {
        if (!Butterfliesfound[i]) {
			ButterfliesAllFound = false;
            return;
        }
    }
    if (ButterfliesAllFound) {
        std::cout << "Congratulations! You found all the butterflies! The sword is now freed" << std::endl;
    }
}
void SceneBasic_Uniform::SetUpNoise() {
    MainShaders.use();

    // generate model for applying noise texture
    mat4 slice = mat4(1.0f);
    slice = glm::rotate(slice, glm::radians(15.0f), vec3(1.0f, 0.0f, 0.0f));
    slice = glm::rotate(slice, glm::radians(-20.0f), vec3(0.0f, 0.0f, 1.0f));
    slice = glm::scale(slice, vec3(2.0f, 2.0f, 1.0f));
    slice = glm::translate(slice, vec3(-0.35f, -0.5f, 1.0f));

    MainShaders.setUniform("Slice", slice);

    // generate noise tecture from helper class (From labs)
    GLuint noiseTex = NoiseTex::generate2DTex();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTex);

    // set thresholds

    MainShaders.setUniform("LowThreshold", 0.4f);
    MainShaders.setUniform("HighThreshold", 0.7f);
}
void SceneBasic_Uniform::SetMatricesDynamic(GLSLProgram &Shader) {
    mat4 mv = view * model;
    Shader.setUniform("ModelViewMatrix", mv);
    Shader.setUniform("NormalMatrix", glm::mat3(mv));
    Shader.setUniform("MVP", projection * mv);
    
}
void SceneBasic_Uniform::DrawSkyBox() {
    //draw sky
    mat4 model = mat4(1.0f);

    glDepthMask(GL_FALSE);    
    glDepthFunc(GL_LEQUAL);

    SkyBoxShaders.use();
    mat4 skyView = mat4(mat3(view));
    mat4 mv = skyView * mat4(1.0f);

    SkyBoxShaders.setUniform("MVP", projection * mv);
    SkyBox.render();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
void SceneBasic_Uniform::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawSkyBox();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    // update light Positions for ground shaders
    GroundShaders.use();
    GroundShaders.setUniform("Light[0].Position", view * lightPos);
    GroundShaders.setUniform("Light[1].Position", view * vec4(0, 0.15f, -1.0f, 0));
    GroundShaders.setUniform("Light[2].Position", view * glm::vec4(-7, 3, 7, 1));
    GroundShaders.setUniform("Light[3].Position", view * glm::vec4(SwordPos + vec3(0.0f, 0.5f, 0.0f)+SwordModifiedPos, 1.0f));
    GroundShaders.setUniform("Light[3].L", vec3(SwordLightIntensity));

    // update light Positions for main shaders
    MainShaders.use();    MainShaders.setUniform("PBRLight[0].Position", view * lightPos);
    MainShaders.setUniform("PBRLight[1].Position", view * vec4(0, 0.15f, -1.0f, 0));
    MainShaders.setUniform("PBRLight[2].Position", view * glm::vec4(-7, 3, 7, 1));
    MainShaders.setUniform("PBRLight[3].Position", view * glm::vec4(SwordPos+vec3(0.0f,0.5f,0.0f)+SwordModifiedPos, 1.0f));
    MainShaders.setUniform("PBRLight[3].L", vec3(SwordLightIntensity));

    drawScene();
    glDisable(GL_POLYGON_OFFSET_FILL);
}
void SceneBasic_Uniform::drawScene() {
   drawFloor();
   DrawAllSwords();
   DrawRock(vec3(-0.0f, -0.5f, 3.0f));
   DrawAllTrees();
   DrawAllButterflies();
}
void SceneBasic_Uniform::MoveSwordAfterButterfliesFound() {
    // moce sword up until it reaches a certain height, then play scene animation
    if (SwordPos.y < 1.5f) {
        SwordPos += vec3(0.0f, 0.0004f, 0.0f);
    }
    else {
        AnimateSword();
    }
    // increase light intensity each frame until it reached 45f
    if(SwordLightIntensity<45.0f){
        SwordLightIntensity += 0.1f;
	}
	
}
void SceneBasic_Uniform::AnimateSword() {
    // make sword bob between 2 heights 
    if (SwordMovingUp) {
		SwordModifiedPos += vec3(0.0f, 0.0009f, 0.0f);
        if(SwordModifiedPos.y>0.4f){
            SwordMovingUp = false;
		}
    }
    else {
        SwordModifiedPos -= vec3(0.0f, 0.0009f, 0.0f);
        if (SwordModifiedPos.y < 0.0f) {
            SwordMovingUp = true;
        }
    }
}
void SceneBasic_Uniform::DrawAllSwords() {
    float metalRough = 0.43f;
    //material for sword 
    drawSword(glm::vec3(SwordPos), metalRough, 1, glm::vec3(0.91f, 0.71f, 0.29f));

    // if butterflies all found call function to update sword
    if (ButterfliesAllFound) {
        MoveSwordAfterButterfliesFound();
    }

}
void SceneBasic_Uniform::drawFloor() {
    // set values for ground then draw plane wirh texture
    GroundShaders.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ForestFloorTexture);
	GroundShaders.setUniform("TextureMixingOn", 0);
    GroundShaders.setUniform("RenderMode", 0);
    GroundShaders.setUniform("EdgeOn", 0);
    GroundShaders.setUniform("Material.Rough", 0.9f);
    GroundShaders.setUniform("Material.Metal", 0);
    GroundShaders.setUniform("Material.Color", glm::vec3(0.2f));

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));

	SetMatricesDynamic(GroundShaders);
    plane.render();
    MainShaders.use();
    MainShaders.setUniform("RenderMode", 1);
}
void SceneBasic_Uniform::DrawRock(const vec3& pos)
{
    // set values for rock then draw
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RockTexture);
    MainShaders.use();
    MainShaders.setUniform("DisintegrationOn", 0);
    MainShaders.setUniform("TextureMixingOn", 0);
    MainShaders.setUniform("EdgeOn", 1);
    MainShaders.setUniform("RenderMode", 0);
    model = mat4(1.0f);

    model = glm::translate(model, pos);
    model = glm::scale(model, vec3(0.02f, //horizontal
        0.03f, // vertical,
        0.01f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    SetMatricesDynamic(MainShaders);
    RockMesh->render();
}
void SceneBasic_Uniform::drawSword(const vec3& pos, float rough, int metal, const vec3& color)
{
    MainShaders.use();
    // if butterflies stil lto be found, display sword with disintegration for damaged effect, if all found, stop displaying disintegration to show sword as repaired
    MainShaders.setUniform("TextureMixingOn", 0);
    if (ButterfliesAllFound) {
        MainShaders.setUniform("DisintegrationOn", 0);
    }
    else {
        MainShaders.setUniform("DisintegrationOn", 1);
    }

    // set values for sword
    MainShaders.setUniform("RenderMode", 1);
    MainShaders.setUniform("ModelMatrix", model);
    MainShaders.setUniform("EdgeOn", 1);
    MainShaders.setUniform("PBRMaterial.Rough", rough);
    MainShaders.setUniform("PBRMaterial.Metal", metal);
    MainShaders.setUniform("PBRMaterial.Color", color);
    model = mat4(1.0f);
    
    // set position for sword then draw
    model = glm::translate(model, pos+SwordModifiedPos);
    model = glm::scale(model, vec3(  0.1f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    MainShaders.setUniform("RenderMode", 1);
    SetMatricesDynamic(MainShaders);
    SwordMesh->render();
}
void SceneBasic_Uniform::DrawButterfly(const vec3& pos,int i) {
    model = mat4(1.0f);

    //set position and shader data for butterfly then draw
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ButterflyTexture);
    MainShaders.setUniform("TextureMixingOn", 0);
    MainShaders.setUniform("DisintegrationOn", 0);
    MainShaders.setUniform("RenderMode", 0);
    MainShaders.setUniform("EdgeOn", 0);

    model = glm::translate(model, pos);
    model=glm::rotate(model, radians(RotationValues[i]), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.005f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));

    SetMatricesDynamic(MainShaders);
    ButterflyMesh->render();
}
void SceneBasic_Uniform::DrawTree(const vec3& pos, const vec3& Scale) {
    model = mat4(1.0f);
    //set position and shader data for tree then draw

    //set textures including mixing texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TreeTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MossTexture);

    MainShaders.setUniform("RenderMode", 0);
    MainShaders.setUniform("TextureMixingOn", 1);
    MainShaders.setUniform("EdgeOn", 0);

    model = glm::translate(model, pos);
    model = glm::scale(model, Scale);

    SetMatricesDynamic(MainShaders);
    TreeMesh->render();

}
void SceneBasic_Uniform::UpdateButterflyPositions() {
    for (int i = 0; i < 5; i++) {
        // move butterflies back and forth
        if (ButterfliesMovingUp[i]) {
            ButterflyModifiedPosition[i] += (vec3(0.0f, ButterflySpeeds[i], 0.0f));
        }
        else {
            ButterflyModifiedPosition[i] -= (vec3(0.0f, ButterflySpeeds[i], 0.0f));
        }
        if (ButterfliesMovingForward[i]) {
            ButterflyModifiedPosition[i] += (vec3( ButterflySpeeds[i],0.0f, 0.0f));
        }
        else {
            ButterflyModifiedPosition[i] -= (vec3( ButterflySpeeds[i],0.0f, 0.0f));
        }
        ///if just changed direction,rotate for 16 frames to do it smoothly
        if (RotationFrames[i] < 16) {
            RotationValues[i] += 11.25;
            RotationFrames[i]++;
        }
        
        // if butterfly hit max/min height, make butterfly start moving opposite way
        if (ButterflyModifiedPosition[i].y > 1.5f) {
            ButterfliesMovingUp[i] = false;
        }
        if (ButterflyModifiedPosition[i].y < 0.5f) {
            ButterfliesMovingUp[i] = true;
        }

        // if butterfly hit max/min distance, make butterfly start moving opposite way
        if (ButterflyModifiedPosition[i].x > 2.5f) {
            ButterfliesMovingForward[i] = false;
            RotationFrames[i] = 0;
        }
        if (ButterflyModifiedPosition[i].x < -2.5f) {
            ButterfliesMovingForward[i] = true;
            RotationFrames[i] = 0;
        }
    }
}
void SceneBasic_Uniform::DrawAllButterflies() {
    
    // update butterflies positions, check if theyre found, and if theyre not, draw them at that position
    UpdateButterflyPositions();
    for(int i=0;i<std::size(ButterflyPositions);i++){
        if (!Butterfliesfound[i]) {
            DrawButterfly(ButterflyPositions[i]+ButterflyModifiedPosition[i],i);
        }
		
	}

	//DrawButterfly(vec3(-2.0f, 0.0f, 2.0f));
}
void SceneBasic_Uniform::DrawAllTrees() {
    // draw trees at various positions/sizes

    DrawTree(vec3(-2.0f, -1.0f, 2.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(2.5f, -1.0f, 2.4f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(-4.0f, -1.0f, 3.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(2.9f, -1.0f, -3.4f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(5.0f, -1.0f, 1.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(-5.0f, -1.0f, -2.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(-3.0f, -1.0f, -1.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(-1.2f, -1.0f, 1.0f), vec3(0.9f, 1.3f, 0.9f));
    DrawTree(vec3(1.5f, -1.0f, 0.5f), vec3(1.1f, 1.6f, 1.1f)); 
    DrawTree(vec3(-3.5f, -1.0f, 4.5f), vec3(0.9f, 1.4f, 0.9f));
    DrawTree(vec3(3.5f, -1.0f, 5.5f), vec3(1.3f, 2.0f, 1.3f));
    DrawTree(vec3(-2.0f, -1.0f, 6.5f), vec3(1.5f, 2.3f, 1.5f));
    DrawTree(vec3(6.0f, -1.0f, 2.5f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(-6.5f, -1.0f, 1.5f), vec3(1.2f, 1.7f, 1.2f));
}
void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;    
	projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
}