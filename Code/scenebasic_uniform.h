#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/torus.h"
#include "helper/teapot.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helper/plane.h"
#include "helper/cube.h"
#include "helper/objmesh.h"
#include "helper/texture.h"
#include "helper/skybox.h"
#include "helper/random.h"
#include "helper/particleutils.h"
#include "helper/grid.h"
#include "helper/frustum.h"
#include "GLFW/glfw3.h";

using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;
using glm::radians;

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram Shaders;
    GLSLProgram SkyBoxShaders;

    SkyBox SkyBox;

    Teapot teapot;
    std::unique_ptr<ObjMesh> SwordMesh;
    std::unique_ptr<ObjMesh> RockMesh;
    Plane plane;

    //textures 
    GLuint RockTexture;


    float tPrev,angle,lightAngle,lightRotationSpeed;
	glm::vec4 lightPos;

    vec3 EyeCoordinates = vec3(0.0f, 0.75f, 6.25f);
    vec3 CameraFront = vec3(0.0f, 0.0f, -1.0f);
    vec3 CameraUp = vec3(0.0f, 1.0f, 0.0f);

    float cameraYaw = -90.0f;
    float cameraPitch = 0.0f;
    bool mouseFirstEntry = true;
    float cameraLastXPos = 800.0f / 2.0f;
    float cameraLastYPos = 600.0f / 2.0f;

    float deltaTime = 0.0f;
   
    float lastFrame = 0.0f;

    void Mouse_CallBack(double Xpos, double Ypos);
    void ProcessUserInput(int key, int action);
    void compile();
public:
    void SetupSkybox();

    void LoadTextures();
    void DrawSkyBox();
    void drawSword(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);
    void DrawRock(const glm::vec3& pos);
    void drawFloor();
    void drawScene();

    void initBuffers();
    float randFloat();
    SceneBasic_Uniform();

    void setMatrices();
    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
