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

using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;

SceneBasic_Uniform::SceneBasic_Uniform() :plane(20,20,1,1),teapot(5,glm::mat4(1.0f)),
tPrev(0.0f),lightPos(5.0f,5.0f,5.0f,1.0f){
    mesh = ObjMesh::load("media/spot/spot_triangulated.obj");
    mesh = ObjMesh::load("media/Tree.obj");
    //mesh = ObjMesh::load("media/swordInStone.obj");
    if (!mesh) {
        cerr << "Failed to load mesh: media/swordInStone.obj" << endl;
    }
}
void SceneBasic_Uniform::initScene()
{
    compile(); 

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

    prog.setUniform("Light[0].L", vec3(45.0f));
	prog.setUniform("Light[0].Position", view * lightPos);
    prog.setUniform("Light[1].L", vec3(0.3f));
    prog.setUniform("Light[1].Position", vec4(0,0.15f,-1.0f,0));
    prog.setUniform("Light[2].L", vec3(45.0f));
    prog.setUniform("Light[2].Position", view * glm::vec4(-7, 3, 7, 1));
}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/NewVertexShader.vert");
        prog.compileShader("shader/NewFragmentShader.frag");
      //  prog.compileShader("shader/Geometry_Shader.gs");
        prog.link();
        prog.use();


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
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(mv));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	prog.setUniform("Light[0].Position", view * lightPos);
    drawScene();

}
void SceneBasic_Uniform::drawScene() {
    drawFloor();

    // draw dielectric cows with varying roughness
    int numCows = 9;
    glm::vec3 cowBaseColor(0.1f, 0.33f, 0.97f);
    for (int i = 0; i < numCows; i++) {
        float cowX = i * (10.0f / (numCows - 1)) - 5.0f;
        float rough = (i + 1) * (1.0f / numCows);
        drawSpot(glm::vec3(cowX, 0.0f, 0.0f), rough, 0, cowBaseColor);
    }

    //draw metal cows
    float metalRough = 0.43f;
    // gold
    drawSpot(glm::vec3(-3.0f, 0.0f, 3.f), metalRough, 1, glm::vec3(1, 0.71f, 0.29f));
    // copper
    drawSpot(glm::vec3(-1.5f, 0.0f, 3.f), metalRough, 1, glm::vec3(0.95f, 0.71f, 0.29f));
    //aluminium
    drawSpot(glm::vec3(-0.f, 0.f, 3.f), metalRough, 1, glm::vec3(0.91f, 0.71f, 0.29f));
    //titanium
    drawSpot(glm::vec3(1.5f, 0.0f, 3.f), metalRough, 1, glm::vec3(0.542f, 0.71f, 0.29f));
    //silver
    drawSpot(glm::vec3(3.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.95f, 0.71f, 0.29f));
    
}
void SceneBasic_Uniform::drawFloor() {
   
    prog.setUniform("Material.Rough", 0.9f);
    prog.setUniform("Material.Metal", 0);
    prog.setUniform("Material.Color", glm::vec3(0.2f));

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));

    setMatrices();
    plane.render();
}
void SceneBasic_Uniform::drawSpot(const vec3& pos, float rough, int metal, const vec3& color)
{
    
    prog.setUniform("Material.Rough", rough);
    prog.setUniform("Material.Metal", metal);
    prog.setUniform("Material.Color", color);
    model = mat4(1.0f);
    
    model = glm::translate(model, pos);
    model = glm::scale(model, vec3(4.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.f, 1.f, 0.f));

    
    setMatrices();
    mesh->render();
}
void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;    
	projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
}