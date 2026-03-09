#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/glslprogram.h"
#include <glad/glad.h>
#include "Camera.h"
#include "ModelLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

enum class PickupPhase
{
    Idle,
    SpinShrink,
    Poof
};

struct EnergyCell
{
    glm::vec3   position;
    glm::vec3   platformPos;
    bool        collected;

    PickupPhase phase;
    float       phaseTimer;

    float       spinAngle;
    float       cellScale;
    float       orbScale;
    float       poofAlpha;

    static constexpr float SPIN_SHRINK_DURATION = 2.2f;
    static constexpr float POOF_DURATION = 1.2f;

    EnergyCell()
        : position(0.f), platformPos(0.f),
        collected(false), phase(PickupPhase::Idle),
        phaseTimer(0.f), spinAngle(0.f),
        cellScale(1.f), orbScale(0.f), poofAlpha(1.f)
    {
    }
};


struct Asteroid
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotationAxis;
    float orbitRadius;
    float angle;
    float angularSpeed;
    float inwardSpeed;
    float heightOffset;
    float tiltAngle;
    float rotationAngle;
    float rotationSpeed;
    float wobblePhase;
    bool  active;

    Asteroid()
        : position(0.f), scale(1.f), rotationAxis(0.f, 1.f, 0.f),
        orbitRadius(0.f), angle(0.f), angularSpeed(0.f), inwardSpeed(0.f),
        heightOffset(0.f), tiltAngle(0.f), rotationAngle(0.f),
        rotationSpeed(0.f), wobblePhase(0.f), active(true)
    {
    }
};

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    GLSLProgram diskProg;
    GLSLProgram skyboxProg;
    GLSLProgram hdrProg;
    GLSLProgram blurProg;
    GLSLProgram platformProg;

    GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
    int    sphereIdxCount = 0;

    GLuint diskVAO = 0, diskVBO = 0;
    int    diskVertCount = 0;

    GLuint platVAO = 0, platVBO = 0;
    int    platVertCount = 0;

    ModelLoader cellModel;
    bool        cellModelLoaded = false;

    GLuint satellitePlatformTex = 0;

    GLuint skyboxVAO = 0, skyboxVBO = 0;
    GLuint skyboxTex = 0;

    GLuint hdrFBO = 0, hdrColorBuf = 0, rboDepth = 0;
    GLuint pingpongFBO[2] = { 0, 0 };
    GLuint pingpongColorbuffers[2] = { 0, 0 };
    GLuint screenVAO = 0, screenVBO = 0;

    Camera camera = Camera(glm::vec3(0.0f, 3.5f, 145.0f));

    glm::vec3 lightPos = glm::vec3(0.0f);
    float     lightOrbitAngle = 0.0f;

    bool  hdrEnabled = true;
    bool  bloomEnabled = true;
    bool  filmMode = false;
    float exposure = 1.35f;

    int width = 800, height = 600;

    glm::vec3 bhPos = glm::vec3(0.0f, 30.0f, -1350.0f);
    float     bhR = 420.0f;

    std::vector<glm::vec3> platformPositions;
    std::vector<EnergyCell> energyCells;
    std::vector<Asteroid> asteroids;

    int   asteroidCount = 320;
    float asteroidBeltInnerRadius = 560.0f;
    float asteroidBeltOuterRadius = 980.0f;
    float asteroidBeltHalfHeight = 52.0f;
    float asteroidConsumeRadius = 120.0f;
    float asteroidPullRadius = 460.0f;

    bool  gameWon = false;
    bool  gameLost = false;
    float dangerLevel = 0.0f;

    float shakeDuration = 0.0f;
    float shakeMagnitude = 0.0f;

    float fadeAlpha = 0.0f;
    float endStateTimer = 0.0f;
    int   fadeState = 0; // 0 none, 1 wait, 2 fade out, 3 fade in

    void compile();
    void buildSphereMesh(int stacks, int slices);
    void buildDiskMesh();
    void buildPlatformMesh();
    void setupSkyboxGeometry();
    void setupScreenQuad();
    void setupHDRFramebuffer();
    GLuint loadCubemap(std::vector<std::string> faces);
    GLuint loadTexture2D(const std::string& path, bool flip = true);

    void resetGame();
    void spawnPlatformsAndCells();
    void initAsteroids();
    void respawnAsteroid(Asteroid& asteroid, bool randomiseAngle = true);
    void updateAsteroids(float dt);
    void renderAsteroids(const glm::mat4& view, const glm::mat4& proj, float t);
    void updateGame(float dt);
    void updatePickupAnimations(float dt);
    void printGameStatus();
    void updateWindowTitle();
    void drawOverlayUI();

    void renderCell(const EnergyCell& cell, const glm::mat4& view,
        const glm::mat4& proj, float t);

    void setPlatformUniforms(const glm::mat4& view, const glm::mat4& proj,
        float t);

public:
    SceneBasic_Uniform();
    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif