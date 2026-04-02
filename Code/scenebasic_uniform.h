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

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;

    Teapot teapot;
    std::unique_ptr<ObjMesh> mesh;
    Plane plane;

    float tPrev,lightAngle,lightRotationSpeed;
	glm::vec4 lightPos;
    
    

    void compile();
public:
    void drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);
    void drawFloor();
    void drawScene();
    void spitOutDepthBuffer();

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
