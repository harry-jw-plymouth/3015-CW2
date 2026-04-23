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



SceneBasic_Uniform::SceneBasic_Uniform() :plane(20,20,1,1),teapot(5,glm::mat4(1.0f)),
tPrev(0.0f),lightPos(5.0f,5.0f,5.0f,1.0f){
    SwordMesh = ObjMesh::load("media/spot/spot_triangulated.obj");
    SwordMesh = ObjMesh::loadWithAdjacency("media/Sword.obj");
    RockMesh = ObjMesh::loadWithAdjacency("media/Rock07-Base.obj");
	TreeMesh = ObjMesh::loadWithAdjacency("media/Tree.obj");
	ButterflyMesh = ObjMesh::loadWithAdjacency("media/Butterfly/_butterfly.obj");
    //mesh = ObjMesh::load("media/swordInStone.obj");

}
void SceneBasic_Uniform::LoadTextures() {
    RockTexture= Texture::loadTexture("media/texture/rock/Rock07-Base-Diffuse.png");
}


void SceneBasic_Uniform::BuildAdjacencies() {

 //   for (unsigned int i = 0; i < .size(); i += 3) {
   //     unsigned int v0 = indices[i];
     //   unsigned int v1 = indices[i + 1];
     //   unsigned int v2 = indices[i + 2];

     //   edgeMap[Edge(v0, v1)].push_back(i);
     //   edgeMap[Edge(v1, v2)].push_back(i);
      //  edgeMap[Edge(v2, v0)].push_back(i);
    //}
}

void SceneBasic_Uniform::Mouse_CallBack(double Xpos, double Ypos) {
    // std::cout << "Moving mouse" << "\n";
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
                //  std::cout << "Key: " << key << std::endl;
            }
            else if (key == GLFW_KEY_A) {
                EyeCoordinates -= normalize(cross(CameraFront, CameraUp)) * movementSpeed;
                //  std::cout << "Key: " << key << std::endl;
            }
            else if (key == GLFW_KEY_S) {
                EyeCoordinates -= movementSpeed * CameraFront;
                //   std::cout << "Key: " << key << std::endl;
            }
            else if (key == GLFW_KEY_D) {
                EyeCoordinates += normalize(cross(CameraFront, CameraUp)) * movementSpeed;


                //std::cout << "Key: " << key << std::endl;
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
    //  GLuint SkyBoxTexture = Texture::loadHdrCubeMap("../Cw1/media/texture/Skybox/Forest/forest-skyboxes/Brudslojan");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBoxTexture);

    Shaders.use();
    
}
void SceneBasic_Uniform::initScene()
{
    compile(); 

    LoadTextures();

    SetupSkybox();

    Shaders.use();
    Shaders.setUniform("RenderType", 0);

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

     Shaders.setUniform("Light[0].L", vec3(45.0f));
	Shaders.setUniform("Light[0].Position", view * lightPos);
    Shaders.setUniform("Light[1].L", vec3(0.3f));
    Shaders.setUniform("Light[1].Position", vec4(0,0.15f,-1.0f,0));
    Shaders.setUniform("Light[2].L", vec3(45.0f));
    Shaders.setUniform("Light[2].Position", view * glm::vec4(-7, 3, 7, 1));


    CombinedShaders.use();
    CombinedShaders.setUniform("PBRLight[0].L", vec3(45.0f));
    CombinedShaders.setUniform("PBRLight[0].Position", view * lightPos);
    CombinedShaders.setUniform("PBRLight[1].L", vec3(0.3f));
    CombinedShaders.setUniform("PBRLight[1].Position", vec4(0, 0.15f, -1.0f, 0));
    CombinedShaders.setUniform("PBRLight[2].L", vec3(45.0f));
    CombinedShaders.setUniform("PBRLight[2].Position", view * glm::vec4(-7, 3, 7, 1));

    //silhouette lines set up 
    
    CombinedShaders.setUniform("Light.Position", view*lightPos);
    CombinedShaders.setUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);

    CombinedShaders.setUniform("Material.Kd", 0.7f, 0.5f, 0.2f);
    CombinedShaders.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);

    CombinedShaders.setUniform("PBRMaterial.Kd", 0.7f, 0.5f, 0.2f);
    CombinedShaders.setUniform("PBRMaterial.Ka", 0.2f, 0.2f, 0.2f);

    CombinedShaders.setUniform("EdgeWidth", 0.02f);
    CombinedShaders.setUniform("PctExtend", 0.07f);
    CombinedShaders.setUniform("LineColor", vec4(0.05f, 0.0f, 0.05f, 1.0f));
   
    Shaders.use();
    
}

void SceneBasic_Uniform::compile()
{
    try {
        CombinedShaders.compileShader("shader/SilhouetteLines.vert");
        CombinedShaders.compileShader("shader/SilhouetteLines.frag");
        CombinedShaders.compileShader("shader/Geometry_Shader.gs");
        CombinedShaders.link();
        

        Shaders.compileShader("shader/MainVertexShader.vert");
        Shaders.compileShader("shader/MainFragmentShader.frag");
        Shaders.link();
        Shaders.use();

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
}
void SceneBasic_Uniform::setMatrices() {
    mat4 mv = view * model;
    Shaders.setUniform("ModelViewMatrix", mv);
    Shaders.setUniform("NormalMatrix", glm::mat3(mv));
    Shaders.setUniform("MVP", projection * mv);
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

    glDepthMask(GL_FALSE);          // disable depth writes
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

    Shaders.use();
	Shaders.setUniform("Light[0].Position", view * lightPos);


    CombinedShaders.use();
    CombinedShaders.setUniform("Light.Position", view * lightPos);
    CombinedShaders.setUniform("PBRLight[0].Position", view * lightPos);

  //  Shaders.use();
    drawScene();

}
void SceneBasic_Uniform::drawScene() {
    drawFloor();
    DrawAllSwords();
    DrawRock(vec3(-0.0f, -0.5f, 3.0f));
    DrawAllTrees();
	DrawAllButterflies();
   // DrawRock(vec3(3.0f, -0.5f, 3.0f));
   
    //
}
void SceneBasic_Uniform::DrawAllSwords() {

    // draw dielectric cows with varying roughness
    int numCows = 9;

    //draw metal cows
    float metalRough = 0.43f;
    // gold
    drawSword(glm::vec3(-3.0f, 0.0f, 3.f), metalRough, 1, glm::vec3(1, 0.71f, 0.29f));
    // copper
    drawSword(glm::vec3(-1.5f, 0.0f, 3.f), metalRough, 1, glm::vec3(0.95f, 0.71f, 0.29f));
    //aluminium
    drawSword(glm::vec3(-0.f, 0.f, 3.f), metalRough, 1, glm::vec3(0.91f, 0.71f, 0.29f));
    //titanium
    drawSword(glm::vec3(1.5f, 0.0f, 3.f), metalRough, 1, glm::vec3(0.542f, 0.71f, 0.29f));
    //silver
    drawSword(glm::vec3(3.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.95f, 0.71f, 0.29f));

}
void SceneBasic_Uniform::drawFloor() {
    Shaders.use();
    Shaders.setUniform("Material.Rough", 0.9f);
    Shaders.setUniform("Material.Metal", 0);
    Shaders.setUniform("Material.Color", glm::vec3(0.2f));

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));

    setMatrices();
    plane.render();
    CombinedShaders.use();
}
void SceneBasic_Uniform::DrawRock(const vec3& pos)
{
  //  Shaders.setUniform("RenderType", 1);
    CombinedShaders.use();

    model = mat4(1.0f);

    model = glm::translate(model, pos);
    model = glm::scale(model, vec3(0.02f, //horizontal
        0.03f, // vertical,
        0.01f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

   // setMatrices();
    SetMatricesDynamic(CombinedShaders);
    RockMesh->render();
    Shaders.use();
    Shaders.setUniform("RenderType", 0); 
}
void SceneBasic_Uniform::drawSword(const vec3& pos, float rough, int metal, const vec3& color)
{
    //Shaders.setUniform("Material.Rough", rough);
   // Shaders.setUniform("Material.Metal", metal);
   // Shaders.setUniform("Material.Color", color);
    CombinedShaders.use();
    CombinedShaders.setUniform("PBRMaterial.Rough", rough);
    CombinedShaders.setUniform("PBRMaterial.Metal", metal);
    CombinedShaders.setUniform("PBRMaterial.Color", color);
    model = mat4(1.0f);
    
    model = glm::translate(model, pos);
    model = glm::scale(model, vec3(  0.1f));
  //  model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    
 //   setMatrices();
    
    SetMatricesDynamic(CombinedShaders);
    SwordMesh->render();
}
void SceneBasic_Uniform::DrawButterfly(const vec3& pos) {
    model = mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, vec3(0.01f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatricesDynamic(CombinedShaders);
    ButterflyMesh->render();
}
void SceneBasic_Uniform::DrawTree(const vec3& pos, const vec3& Scale) {
    model = mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, Scale);
    //model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatricesDynamic(CombinedShaders);
    TreeMesh->render();
}
void SceneBasic_Uniform::DrawAllButterflies() {
	
}
void SceneBasic_Uniform::DrawAllTrees() {
    DrawTree(vec3(-2.0f, -1.0f, 2.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(2.5f, -1.0f, 2.4f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(-4.0f, -1.0f, 3.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(2.9f, -1.0f, -3.4f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(5.0f, -1.0f, 1.0f), vec3(1.0f, 1.5f, 1.0f));
    DrawTree(vec3(-5.0f, -1.0f, -2.0f), vec3(1.0f, 1.5f, 1.0f));
}
void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;    
	projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
}