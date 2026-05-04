#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <list>

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
#include "helper/noisetex.h";

using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;
using glm::radians;

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram GroundShaders;
    GLSLProgram MainShaders;
    GLSLProgram SkyBoxShaders;

    SkyBox SkyBox;

    std::unique_ptr<ObjMesh> SwordMesh;
    std::unique_ptr<ObjMesh> RockMesh;
    std::unique_ptr<ObjMesh> ButterflyMesh;
    std::unique_ptr<ObjMesh> TreeMesh;
	std::unique_ptr<ObjMesh > TestMesh;
    Plane plane;

    //textures 
    GLuint RockTexture;
	GLuint TreeTexture;
    GLuint ForestFloorTexture;
	GLuint ButterflyTexture;
	GLuint MossTexture;

    float tPrev,angle,lightAngle,lightRotationSpeed;
	glm::vec4 lightPos;

    bool SwordMovingUp = true;

    vec3 SwordPos= vec3( - 0.f, 0.f, 3.f);
    vec3 SwordModifiedPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 EyeCoordinates = vec3(0.0f, 0.75f, 6.25f);
    vec3 CameraFront = vec3(0.0f, 0.0f, -1.0f);
    vec3 CameraUp = vec3(0.0f, 1.0f, 0.0f);

    float cameraYaw = -90.0f;
    float cameraPitch = 0.0f;
    bool mouseFirstEntry = true;
    float cameraLastXPos = 800.0f / 2.0f;
    float cameraLastYPos = 600.0f / 2.0f;

	bool ButterfliesAllFound = false;

    int NumberOfButterFlies = 5;
    vec3 ButterflyPositions[5] = {
        vec3(5.0f, 0.0f, 5.0f),
        vec3(2.0f, 0.0f, 6.0f),
        vec3(-2.0f, 0.0f, -5.0f),
        vec3(3.0f, 0.0f, 9.0f),
        vec3(-6.0f, 0.0f, 8.0f)
    };
	bool Butterfliesfound[5] = { false,false,false,false,false };

    bool ButterfliesMovingForward[5] = { true,true,false,false,true };
    bool ButterfliesMovingUp[5] = { false,true,false,true,false };

    vec3 ButterflyModifiedPosition[5] = {
        vec3(0.0f,1.3f,0.0f),
        vec3(0.0f,1.1f,0.0f),
        vec3(0.0f,0.9f,0.0f),
        vec3(0.0f,1.5f,0.0f),
        vec3(0.0f,0.5f,0.0f)
    };

    float RotationValues[5] = {
        0.0f,0.0f,180.0f,180.0f,0.0f
    };
    int RotationFrames[5] = {
        16,16,16,16,16
    };

    float ButterflySpeeds[5] = {
        0.009,0.008,0.0085,0.0092,0.0087
    };



    float deltaTime = 0.0f;
	float SwordLightIntensity = 0.0f;
   
    float lastFrame = 0.0f;

    void Mouse_CallBack(double Xpos, double Ypos);
    void ProcessUserInput(int key, int action);
    void compile();
public:
    void UpdateButterflyPositions();
    void SetUpNoise();
	void AnimateSword();
	void MoveSwordAfterButterfliesFound();
	void CheckIfAllButerfliesFound();
	void CheckForButterflyCollisions();
    void SetupSkybox();
    void SetMatricesDynamic(GLSLProgram &Shader);

    void LoadTextures();
    void DrawSkyBox();
    void drawSword(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);
    void DrawRock(const glm::vec3& pos);
    void DrawAllButterflies();
    void DrawAllTrees();
    void DrawAllSwords();
    void DrawTree(const glm::vec3& pos, const vec3& Scale);
    void DrawButterfly(const glm::vec3& pos,int i);
    void drawFloor();
    void drawScene();

    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
