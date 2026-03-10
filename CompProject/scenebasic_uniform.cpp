#include "scenebasic_uniform.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "helper/stb/stb_image.h"

// Skybox cube vertices
static float s_skyboxVerts[] = {
    -1, 1,-1, -1,-1,-1,  1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
    -1,-1, 1, -1,-1,-1, -1, 1,-1, -1, 1,-1, -1, 1, 1, -1,-1, 1,
     1,-1,-1,  1,-1, 1,  1, 1, 1,  1, 1, 1,  1, 1,-1,  1,-1,-1,
    -1,-1, 1, -1, 1, 1,  1, 1, 1,  1, 1, 1,  1,-1, 1, -1,-1, 1,
    -1, 1,-1,  1, 1,-1,  1, 1, 1,  1, 1, 1, -1, 1, 1, -1, 1,-1,
    -1,-1,-1, -1,-1, 1,  1,-1,-1,  1,-1,-1, -1,-1, 1,  1,-1, 1
};

static float randf(float minVal, float maxVal)
{
    float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return minVal + t * (maxVal - minVal);
}

SceneBasic_Uniform::SceneBasic_Uniform() {}

static void LogInfo(const std::string& msg)
{
    std::cout << "[INFO] " << msg << std::endl;
}

static void LogWarn(const std::string& msg)
{
    std::cout << "[WARN] " << msg << std::endl;
}

static void LogError(const std::string& msg)
{
    std::cerr << "[ERROR] " << msg << std::endl;
}

void SceneBasic_Uniform::compile()
{
    try {
        LogInfo("Compiling black hole shader...");
        prog.compileShader("shader/blackhole.vert");
        prog.compileShader("shader/blackhole.frag");
        prog.link();

        LogInfo("Compiling accretion disk shader...");
        diskProg.compileShader("shader/disk.vert");
        diskProg.compileShader("shader/disk.frag");
        diskProg.link();

        LogInfo("Compiling skybox shader...");
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();

        LogInfo("Compiling HDR shader...");
        hdrProg.compileShader("shader/hdr.vert");
        hdrProg.compileShader("shader/hdr.frag");
        hdrProg.link();

        LogInfo("Compiling blur shader...");
        blurProg.compileShader("shader/hdr.vert");
        blurProg.compileShader("shader/blur.frag");
        blurProg.link();

        LogInfo("Compiling platform shader...");
        platformProg.compileShader("shader/platform.vert");
        platformProg.compileShader("shader/platform.frag");
        platformProg.link();

        LogInfo("All shaders compiled and linked successfully.");
    }
    catch (GLSLProgramException& e) {
        LogError(std::string("Shader compilation/linking failed: ") + e.what());
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::buildSphereMesh(int stacks, int slices)
{
    std::vector<float> verts;
    std::vector<unsigned int> indices;
    const float PI = 3.14159265f;
    const float PI2 = 6.28318530f;

    for (int i = 0; i <= stacks; i++) {
        float phi = PI * (float)i / stacks;
        float sinPhi = sinf(phi), cosPhi = cosf(phi);

        for (int j = 0; j <= slices; j++) {
            float theta = PI2 * (float)j / slices;
            float x = sinPhi * cosf(theta);
            float y = cosPhi;
            float z = sinPhi * sinf(theta);

            float tx = -sinf(theta);
            float ty = 0.0f;
            float tz = cosf(theta);
            float bx = cosPhi * cosf(theta);
            float by = -sinPhi;
            float bz = cosPhi * sinf(theta);

            verts.insert(verts.end(), {
                x, y, z,
                x, y, z,
                (float)j / slices, (float)i / stacks,
                tx, ty, tz,
                bx, by, bz
                });
        }
    }

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            int r1 = i * (slices + 1);
            int r2 = (i + 1) * (slices + 1);
            indices.push_back(r1 + j);
            indices.push_back(r2 + j);
            indices.push_back(r2 + j + 1);
            indices.push_back(r1 + j);
            indices.push_back(r2 + j + 1);
            indices.push_back(r1 + j + 1);
        }
    }

    sphereIdxCount = (int)indices.size();
    const int stride = 14;

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

void SceneBasic_Uniform::buildDiskMesh()
{
    std::vector<float> verts;
    const int rings = 120;
    const int slices = 300;
    const float inner = 1.02f;
    const float outer = 3.2f;
    const float PI2 = 6.28318530f;

    for (int i = 0; i < rings; i++) {
        float r0 = inner + (outer - inner) * (float)i / rings;
        float r1 = inner + (outer - inner) * (float)(i + 1) / rings;

        for (int j = 0; j < slices; j++) {
            float a0 = (float)j / slices * PI2;
            float a1 = (float)(j + 1) / slices * PI2;

            glm::vec3 p00(cosf(a0) * r0, 0, sinf(a0) * r0);
            glm::vec3 p10(cosf(a1) * r0, 0, sinf(a1) * r0);
            glm::vec3 p01(cosf(a0) * r1, 0, sinf(a0) * r1);
            glm::vec3 p11(cosf(a1) * r1, 0, sinf(a1) * r1);

            verts.insert(verts.end(), { p00.x,p00.y,p00.z });
            verts.insert(verts.end(), { p10.x,p10.y,p10.z });
            verts.insert(verts.end(), { p11.x,p11.y,p11.z });
            verts.insert(verts.end(), { p00.x,p00.y,p00.z });
            verts.insert(verts.end(), { p11.x,p11.y,p11.z });
            verts.insert(verts.end(), { p01.x,p01.y,p01.z });
        }
    }

    diskVertCount = (int)verts.size() / 3;

    glGenVertexArrays(1, &diskVAO);
    glGenBuffers(1, &diskVBO);

    glBindVertexArray(diskVAO);
    glBindBuffer(GL_ARRAY_BUFFER, diskVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void SceneBasic_Uniform::buildPlatformMesh()
{
    std::vector<float> verts;
    const int sides = 6;
    const float R = 6.0f;
    const float H = 0.3f;
    const float PI2 = 6.28318530f;

    auto pushVert = [&](glm::vec3 pos, glm::vec3 n, glm::vec2 uv, glm::vec3 tangent)
        {
            glm::vec3 bt = glm::normalize(glm::cross(n, tangent));
            verts.insert(verts.end(), {
                pos.x, pos.y, pos.z,
                n.x,   n.y,   n.z,
                uv.x,  uv.y,
                tangent.x, tangent.y, tangent.z,
                bt.x, bt.y, bt.z
                });
        };

    for (int i = 0; i < sides; i++) {
        float a0 = (float)i / sides * PI2;
        float a1 = (float)(i + 1) / sides * PI2;

        glm::vec3 c(0, H, 0);
        glm::vec3 p0(cosf(a0) * R, H, sinf(a0) * R);
        glm::vec3 p1(cosf(a1) * R, H, sinf(a1) * R);
        glm::vec3 n(0, 1, 0);
        glm::vec3 t(1, 0, 0);

        pushVert(c, n, { 0.5f, 0.5f }, t);
        pushVert(p0, n, { cosf(a0) * .5f + .5f, sinf(a0) * .5f + .5f },
            glm::normalize(glm::vec3(cosf(a0), 0, sinf(a0))));
        pushVert(p1, n, { cosf(a1) * .5f + .5f, sinf(a1) * .5f + .5f },
            glm::normalize(glm::vec3(cosf(a1), 0, sinf(a1))));
    }

    for (int i = 0; i < sides; i++) {
        float a0 = (float)i / sides * PI2;
        float a1 = (float)(i + 1) / sides * PI2;

        glm::vec3 tl(cosf(a0) * R, H, sinf(a0) * R);
        glm::vec3 tr(cosf(a1) * R, H, sinf(a1) * R);
        glm::vec3 bl(cosf(a0) * R, 0, sinf(a0) * R);
        glm::vec3 br(cosf(a1) * R, 0, sinf(a1) * R);

        float am = (a0 + a1) * .5f;
        glm::vec3 n(cosf(am), 0, sinf(am));
        glm::vec3 t = glm::normalize(glm::vec3(-sinf(am), 0, cosf(am)));

        pushVert(tl, n, { 0,1 }, t);
        pushVert(bl, n, { 0,0 }, t);
        pushVert(br, n, { 1,0 }, t);
        pushVert(tl, n, { 0,1 }, t);
        pushVert(br, n, { 1,0 }, t);
        pushVert(tr, n, { 1,1 }, t);
    }

    platVertCount = (int)verts.size() / 14;

    glGenVertexArrays(1, &platVAO);
    glGenBuffers(1, &platVBO);

    glBindVertexArray(platVAO);
    glBindBuffer(GL_ARRAY_BUFFER, platVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    const GLsizei stride = 14 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

void SceneBasic_Uniform::setupSkyboxGeometry()
{
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_skyboxVerts), s_skyboxVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void SceneBasic_Uniform::setupScreenQuad()
{
    float v[] = {
        -1,  1, 0, 1,
        -1, -1, 0, 0,
         1, -1, 1, 0,
        -1,  1, 0, 1,
         1, -1, 1, 0,
         1,  1, 1, 1
    };

    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);

    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void SceneBasic_Uniform::setupHDRFramebuffer()
{
    LogInfo("Creating HDR framebuffer at " + std::to_string(width) + "x" + std::to_string(height));

    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glGenTextures(1, &hdrColorBuf);
    glBindTexture(GL_TEXTURE_2D, hdrColorBuf);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBuf, 0);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LogError("HDR framebuffer incomplete.");
    else
        LogInfo("HDR framebuffer complete.");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);

        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            LogError("Ping-pong framebuffer incomplete: index " + std::to_string(i));
        else
            LogInfo("Ping-pong framebuffer complete: index " + std::to_string(i));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint SceneBasic_Uniform::loadCubemap(std::vector<std::string> faces)
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < faces.size(); i++) {
        int w, h, ch;
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &ch, 0);
        if (data) {
            GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);

            LogInfo("Cubemap face loaded: " + faces[i] +
                " | size=" + std::to_string(w) + "x" + std::to_string(h) +
                " | channels=" + std::to_string(ch));
        }
        else {
            LogError("Cubemap failed: " + faces[i]);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return id;
}

GLuint SceneBasic_Uniform::loadTexture2D(const std::string& path, bool flip)
{
    GLuint tex = 0;
    glGenTextures(1, &tex);

    int w, h, ch;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);

    if (!data) {
        LogError("Texture failed: " + path);
        glDeleteTextures(1, &tex);
        return 0;
    }

    LogInfo("Texture loaded: " + path +
        " | size=" + std::to_string(w) + "x" + std::to_string(h) +
        " | channels=" + std::to_string(ch));

    GLenum fmt = GL_RGB;
    if (ch == 1) fmt = GL_RED;
    else if (ch == 3) fmt = GL_RGB;
    else if (ch == 4) fmt = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
    return tex;
}

void SceneBasic_Uniform::initAsteroids()
{
    asteroids.clear();
    asteroids.resize(asteroidCount);

    for (int i = 0; i < asteroidCount; i++)
        respawnAsteroid(asteroids[i], true);

    std::cout << "[Spawn] " << asteroidCount << " asteroids seeded into the belt.\n";
}

void SceneBasic_Uniform::respawnAsteroid(Asteroid& asteroid, bool randomiseAngle)
{
    asteroid.orbitRadius = randf(asteroidBeltInnerRadius, asteroidBeltOuterRadius);

    if (randomiseAngle || asteroid.angle > glm::two_pi<float>())
        asteroid.angle = randf(0.0f, glm::two_pi<float>());

    asteroid.heightOffset = randf(-asteroidBeltHalfHeight, asteroidBeltHalfHeight);

    float radiusT = 1.0f - ((asteroid.orbitRadius - asteroidBeltInnerRadius) /
        (asteroidBeltOuterRadius - asteroidBeltInnerRadius));

    asteroid.angularSpeed = randf(0.08f, 0.18f) + radiusT * randf(0.05f, 0.16f);
    asteroid.inwardSpeed = randf(8.0f, 26.0f);

    float baseScale = randf(1.0f, 6.5f);
    asteroid.scale = glm::vec3(
        baseScale * randf(0.60f, 1.45f),
        baseScale * randf(0.55f, 1.30f),
        baseScale * randf(0.65f, 1.40f));

    asteroid.rotationAxis = glm::normalize(glm::vec3(
        randf(-1.0f, 1.0f),
        randf(-1.0f, 1.0f),
        randf(-1.0f, 1.0f)));

    if (glm::length(asteroid.rotationAxis) < 0.001f)
        asteroid.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

    asteroid.tiltAngle = randf(-32.0f, 32.0f);
    asteroid.rotationAngle = randf(0.0f, 360.0f);
    asteroid.rotationSpeed = randf(8.0f, 70.0f);
    asteroid.wobblePhase = randf(0.0f, glm::two_pi<float>());

    asteroid.position = bhPos + glm::vec3(
        cosf(asteroid.angle) * asteroid.orbitRadius,
        asteroid.heightOffset,
        sinf(asteroid.angle) * asteroid.orbitRadius);

    asteroid.active = true;
}

void SceneBasic_Uniform::updateAsteroids(float dt)
{
    if (asteroids.empty()) return;

    for (auto& asteroid : asteroids)
    {
        if (!asteroid.active) continue;

        glm::vec3 offset = asteroid.position - bhPos;
        float dist = glm::length(offset);
        if (dist < 0.001f) dist = 0.001f;

        float pullT = 1.0f - glm::clamp((dist - asteroidConsumeRadius) /
            glm::max(0.001f, asteroidPullRadius - asteroidConsumeRadius), 0.0f, 1.0f);
        pullT = glm::clamp(pullT, 0.0f, 1.0f);

        asteroid.angle += asteroid.angularSpeed * dt * (1.0f + pullT * 1.65f);

        float inwardThisFrame = asteroid.inwardSpeed * dt;
        inwardThisFrame += pullT * pullT * 125.0f * dt;
        asteroid.orbitRadius -= inwardThisFrame;

        float wobble = sinf((float)glfwGetTime() * (0.35f + asteroid.angularSpeed) + asteroid.wobblePhase)
            * (4.0f + asteroid.scale.x * 0.25f);
        float collapse = pullT * 18.0f;
        if (asteroid.heightOffset > 0.0f)
            asteroid.heightOffset = glm::max(0.0f, asteroid.heightOffset - collapse * dt);
        else
            asteroid.heightOffset = glm::min(0.0f, asteroid.heightOffset + collapse * dt);

        asteroid.position = bhPos + glm::vec3(
            cosf(asteroid.angle) * asteroid.orbitRadius,
            asteroid.heightOffset + wobble,
            sinf(asteroid.angle) * asteroid.orbitRadius);

        glm::vec3 inwardDir = glm::normalize(bhPos - asteroid.position);
        asteroid.position += inwardDir * (pullT * 55.0f * dt);

        asteroid.rotationAngle += asteroid.rotationSpeed * dt * (1.0f + pullT);
        if (asteroid.rotationAngle > 360.0f)
            asteroid.rotationAngle = fmodf(asteroid.rotationAngle, 360.0f);

        if (asteroid.orbitRadius <= asteroidConsumeRadius ||
            glm::length(asteroid.position - bhPos) <= asteroidConsumeRadius)
        {
            respawnAsteroid(asteroid, true);
        }
    }
}

void SceneBasic_Uniform::renderAsteroids(const glm::mat4& view, const glm::mat4& proj, float t)
{
    if (asteroids.empty()) return;

    setPlatformUniforms(view, proj, t);

    platformProg.use();
    platformProg.setUniform("uIsCore", 0);
    platformProg.setUniform("uHasDiffuse", 0);
    platformProg.setUniform("uHasNormal", 0);

    glBindVertexArray(sphereVAO);

    for (const auto& asteroid : asteroids)
    {
        if (!asteroid.active) continue;

        float dist = glm::length(asteroid.position - bhPos);
        float pullT = 1.0f - glm::clamp((dist - asteroidConsumeRadius) /
            glm::max(0.001f, asteroidPullRadius - asteroidConsumeRadius), 0.0f, 1.0f);
        pullT = glm::clamp(pullT, 0.0f, 1.0f);

        glm::vec3 baseColor = glm::mix(glm::vec3(0.18f, 0.17f, 0.16f),
            glm::vec3(0.34f, 0.25f, 0.18f), pullT * 0.55f);
        glm::vec3 glowColor = glm::mix(glm::vec3(0.06f, 0.05f, 0.04f),
            glm::vec3(1.25f, 0.48f, 0.12f), pullT * 0.9f);
        float glowStrength = 0.10f + pullT * 0.95f;

        glm::mat4 model(1.0f);
        model = glm::translate(model, asteroid.position);
        model = glm::rotate(model, asteroid.angle + glm::radians(asteroid.tiltAngle), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(asteroid.rotationAngle), asteroid.rotationAxis);
        model = glm::scale(model, asteroid.scale);

        platformProg.setUniform("Model", model);
        platformProg.setUniform("uBaseColor", baseColor);
        platformProg.setUniform("uGlowColor", glowColor);
        platformProg.setUniform("uGlowStrength", glowStrength);
        platformProg.setUniform("uLightPos2", bhPos);
        platformProg.setUniform("uLightColor2", glm::vec3(1.25f, 0.52f, 0.12f));
        platformProg.setUniform("uLightStrength2", 0.24f + pullT * 0.95f);

        glDrawElements(GL_TRIANGLES, sphereIdxCount, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
}

void SceneBasic_Uniform::spawnPlatformsAndCells()
{
    platformPositions.clear();
    energyCells.clear();

    srand((unsigned int)time(nullptr));

    int count = 3 + (rand() % 3);

    const float PI2 = 6.28318530f;
    const float baseAngleStep = PI2 / (float)count;
    const float minDist = 55.0f;
    const float maxDist = 120.0f;
    const float platformY = 0.8f;

    for (int i = 0; i < count; i++)
    {
        float jitter = ((float)(rand() % 1000) / 1000.f - 0.5f) * baseAngleStep * 0.6f;
        float angle = (float)i * baseAngleStep + jitter;

        float dist = minDist + ((float)(rand() % 1000) / 1000.f) * (maxDist - minDist);
        float yOff = ((float)(rand() % 1000) / 1000.f - 0.5f) * 6.0f;

        glm::vec3 platPos(cosf(angle) * dist, platformY + yOff, sinf(angle) * dist);
        platformPositions.push_back(platPos);

        EnergyCell cell;
        cell.platformPos = platPos;
        cell.position = platPos + glm::vec3(0.0f, 2.5f, 0.0f);
        energyCells.push_back(cell);
    }

    LogInfo("Spawned " + std::to_string(count) + " satellite platforms and " +
        std::to_string(energyCells.size()) + " energy cells.");
}

void SceneBasic_Uniform::resetGame()
{
    camera = Camera(glm::vec3(0.0f, 3.5f, 145.0f));
    camera.firstMouse = true;

    gameWon = false;
    gameLost = false;
    dangerLevel = 0.0f;
    exposure = 1.35f;

    spawnPlatformsAndCells();
    initAsteroids();

    LogInfo("Game reset.");
    LogInfo("Camera reset to start position.");
    LogInfo("Exposure reset to " + std::to_string(exposure));

    std::cout << "\n=== BLACK HOLE MISSION RESET ===\n"
        << "Collect all energy cells and return to the platform centre.\n"
        << "Controls: WASD + Mouse | H HDR | B Bloom | F Film | Q/E Exposure | Left Mouse Collect | G Reset\n"
        << "Avoid getting pulled too close to the black hole.\n\n";
}

void SceneBasic_Uniform::printGameStatus()
{
    int collected = 0;
    for (const auto& c : energyCells) if (c.collected) collected++;

    std::cout << "Cells: " << collected << "/" << energyCells.size()
        << " | Exposure: " << exposure
        << " | Danger: " << dangerLevel
        << " | Film: " << (filmMode ? "ON" : "OFF") << "\n";
}

void SceneBasic_Uniform::updateWindowTitle()
{
    GLFWwindow* w = glfwGetCurrentContext();
    if (!w) return;

    int collected = 0;
    for (const auto& c : energyCells) if (c.collected) collected++;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (gameLost)
    {
        oss << "BLACK HOLE MISSION | FAILED | Resetting... | Cells "
            << collected << "/" << energyCells.size()
            << " | Danger " << dangerLevel;
    }
    else if (gameWon)
    {
        oss << "BLACK HOLE MISSION | COMPLETE | Resetting... | Cells "
            << collected << "/" << energyCells.size();
    }
    else
    {
        oss << "BLACK HOLE MISSION | Objective: Collect all cells and return to base | "
            << "Cells " << collected << "/" << energyCells.size()
            << " | Danger " << dangerLevel
            << " | Exposure " << exposure;
    }

    glfwSetWindowTitle(w, oss.str().c_str());
}

void SceneBasic_Uniform::initScene()
{
    LogInfo("Initialising scene...");
    compile();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    buildSphereMesh(128, 128);
    LogInfo("Sphere mesh built.");

    buildDiskMesh();
    LogInfo("Disk mesh built.");

    buildPlatformMesh();
    LogInfo("Platform mesh built.");

    setupSkyboxGeometry();
    LogInfo("Skybox geometry ready.");

    setupScreenQuad();
    LogInfo("Screen quad ready.");

    setupHDRFramebuffer();
    LogInfo("HDR framebuffer setup finished.");

    skyboxTex = loadCubemap({
        "media/skybox/right.png",  "media/skybox/left.png",
        "media/skybox/top.png",    "media/skybox/bottom.png",
        "media/skybox/front.png",  "media/skybox/back.png"
        });

    if (!skyboxTex)
        LogWarn("Skybox cubemap failed to load.");

    satellitePlatformTex = loadTexture2D("media/blue_metal_plate_diff_4k.jpg", true);
    if (!satellitePlatformTex)
        LogWarn("Satellite platform texture missing. Using flat colour.");

    cellModelLoaded = cellModel.load("media/cell.obj");
    if (!cellModelLoaded)
        LogWarn("cell.obj missing. Falling back to procedural sphere.");

    GLFWwindow* win = glfwGetCurrentContext();
    if (!win) {
        LogError("GLFW current context is null in initScene().");
        return;
    }

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glViewport(0, 0, width, height);
    LogInfo("Viewport initialised to " + std::to_string(width) + "x" + std::to_string(height));

    resetGame();
}

void SceneBasic_Uniform::updatePickupAnimations(float dt)
{
    for (auto& cell : energyCells)
    {
        if (cell.collected || cell.phase == PickupPhase::Idle) continue;

        cell.phaseTimer += dt;

        if (cell.phase == PickupPhase::SpinShrink)
        {
            float p = glm::clamp(cell.phaseTimer / EnergyCell::SPIN_SHRINK_DURATION, 0.f, 1.f);

            cell.spinAngle += dt * (4.0f + p * p * 40.0f);
            cell.cellScale = 1.0f - glm::smoothstep(0.6f, 1.0f, p);
            cell.poofAlpha = 1.0f;
            cell.orbScale = glm::smoothstep(0.2f, 1.0f, p) * 0.5f;

            if (p >= 1.0f) {
                cell.phase = PickupPhase::Poof;
                cell.phaseTimer = 0.0f;
                cell.cellScale = 0.0f;
                cell.orbScale = 0.5f;
                cell.poofAlpha = 1.0f;
                shakeDuration = 0.45f;
                shakeMagnitude = 0.18f;
            }
        }
        else if (cell.phase == PickupPhase::Poof)
        {
            float p = glm::clamp(cell.phaseTimer / EnergyCell::POOF_DURATION, 0.f, 1.f);
            cell.orbScale = 0.5f + p * 0.5f;
            cell.poofAlpha = 1.0f - (p * p);

            if (p >= 1.0f)
                cell.collected = true;
        }
    }
}

void SceneBasic_Uniform::updateGame(float dt)
{
    if (gameWon || gameLost) return;

    if (dt < 0.0f) {
        LogWarn("Negative dt detected in updateGame(). Clamping may be needed.");
    }

    glm::vec3 toBH = bhPos - camera.Position;
    float distToBH = glm::length(toBH);

    if (distToBH < 0.0001f) {
        LogWarn("Camera extremely close to black hole centre; normalize may become unstable.");
        return;
    }

    glm::vec3 bhDir = glm::normalize(toBH);

    const glm::vec3 mainPlatformPos(0.0f, 1.0f, 145.0f);

    const float safeRadius = 9.0f;
    float closestPlatDist = glm::length(camera.Position - mainPlatformPos);
    for (const auto& pp : platformPositions)
        closestPlatDist = glm::min(closestPlatDist, glm::length(camera.Position - pp));

    float platInfluence = glm::clamp((closestPlatDist - safeRadius) / safeRadius, 0.f, 1.f);

    float gravityStartR = bhR + 1500.0f;
    float lethalR = bhR + 180.0f;

    float normDist = glm::clamp((distToBH - lethalR) / (gravityStartR - lethalR), 0.f, 1.f);
    float proximity = 1.0f - normDist;
    proximity = pow(proximity, 1.35f);

    float platformShield = glm::mix(0.28f, 1.0f, platInfluence);
    dangerLevel = glm::clamp(proximity * glm::mix(0.45f, 1.0f, platInfluence), 0.f, 1.f);

    if (dangerLevel > 0.95f && !gameWon && !gameLost) {
        static float lastDangerLog = -10.0f;
        float now = (float)glfwGetTime();
        if (now - lastDangerLog > 1.5f) {
            LogWarn("Danger critical: " + std::to_string(dangerLevel));
            lastDangerLog = now;
        }
    }

    float pullSpeed = (1.8f + proximity * proximity * 42.0f) * platformShield;
    float redZoneBoost = 1.0f + glm::smoothstep(0.78f, 1.0f, dangerLevel) * 10.0f;
    pullSpeed *= redZoneBoost;

    camera.Position += bhDir * pullSpeed * dt;

    const float collectRange = 8.0f;
    static float lastHintTime = -10.0f;
    float nowTime = (float)glfwGetTime();

    for (const auto& cell : energyCells)
    {
        if (cell.phase != PickupPhase::Idle) continue;

        if (glm::length(camera.Position - cell.position) < collectRange)
        {
            if (nowTime - lastHintTime > 3.0f)
            {
                std::cout << "[Left Mouse] Click to collect energy cell.\n";
                lastHintTime = nowTime;
            }
            break;
        }
    }

    updatePickupAnimations(dt);

    bool allDone = true;
    for (const auto& c : energyCells)
    {
        if (!c.collected) {
            allDone = false;
            break;
        }
    }

    if (distToBH < lethalR) {
        gameLost = true;
        fadeState = 1;
        endStateTimer = 1.0f;
        std::cout << "\n*** MISSION FAILED: Consumed by the black hole. ***\n\n";
    }

    if (allDone && glm::length(camera.Position - glm::vec3(0, 2, 145.0f)) < 4.5f) {
        gameWon = true;
        fadeState = 1;
        endStateTimer = 1.0f;
        std::cout << "\n*** MISSION COMPLETE: All energy cells recovered! ***\n\n";
    }
}

void SceneBasic_Uniform::update(float t)
{
    static float lastT = 0.0f;
    float dt = (lastT == 0.0f) ? 0.0f : t - lastT;
    dt = glm::clamp(dt, 0.0f, 0.1f);
    lastT = t;

    GLFWwindow* w = glfwGetCurrentContext();
    if (!w) {
        LogError("GLFW window/context is null in update().");
        return;
    }

    if (dt >= 0.099f) {
        LogWarn("Large frame dt detected: " + std::to_string(dt));
    }

    if (fadeState == 0 || fadeState == 3)
    {
        camera.processKeyboard(w);
        camera.processMouse(w);
    }

    lightOrbitAngle += 0.35f * dt;
    lightPos = glm::vec3(
        sinf(lightOrbitAngle) * 18.0f,
        6.0f + sinf(lightOrbitAngle * 0.5f) * 4.0f,
        -60.0f + cosf(lightOrbitAngle) * 18.0f
    );

    static bool hPrev = false, bPrev = false, fPrev = false,
        qPrev = false, ePrev = false, gPrev = false, lmbPrev = false;

    bool hNow = glfwGetKey(w, GLFW_KEY_H) == GLFW_PRESS;
    if (hNow && !hPrev) {
        hdrEnabled = !hdrEnabled;
        LogInfo(std::string("HDR toggled: ") + (hdrEnabled ? "ON" : "OFF"));
    }
    hPrev = hNow;

    bool bNow = glfwGetKey(w, GLFW_KEY_B) == GLFW_PRESS;
    if (bNow && !bPrev) {
        bloomEnabled = !bloomEnabled;
        LogInfo(std::string("Bloom toggled: ") + (bloomEnabled ? "ON" : "OFF"));
    }
    bPrev = bNow;

    bool fNow = glfwGetKey(w, GLFW_KEY_F) == GLFW_PRESS;
    if (fNow && !fPrev) {
        filmMode = !filmMode;
        LogInfo(std::string("Film Mode toggled: ") + (filmMode ? "ON" : "OFF"));
    }
    fPrev = fNow;

    bool qNow = glfwGetKey(w, GLFW_KEY_Q) == GLFW_PRESS;
    if (qNow && !qPrev) {
        exposure = glm::max(0.15f, exposure - 0.25f);
        LogInfo("Exposure changed: " + std::to_string(exposure));
    }
    qPrev = qNow;

    bool eNow = glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS;
    if (eNow && !ePrev) {
        exposure = glm::min(5.0f, exposure + 0.25f);
        LogInfo("Exposure changed: " + std::to_string(exposure));
    }
    ePrev = eNow;

    bool lmbNow = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (lmbNow && !lmbPrev && !gameWon && !gameLost)
    {
        const float collectRange = 8.0f;
        EnergyCell* nearest = nullptr;
        float nearestDist = collectRange;

        for (auto& cell : energyCells)
        {
            if (cell.phase != PickupPhase::Idle) continue;
            float d = glm::length(camera.Position - cell.position);
            if (d < nearestDist) {
                nearestDist = d;
                nearest = &cell;
            }
        }

        if (nearest)
        {
            nearest->phase = PickupPhase::SpinShrink;
            nearest->phaseTimer = 0.0f;

            int picked = 1;
            for (const auto& c : energyCells)
                if (&c != nearest && (c.collected || c.phase != PickupPhase::Idle)) picked++;

            std::cout << "Energy cell collected! (" << picked
                << "/" << energyCells.size() << ")\n";
            printGameStatus();
        }
        else
        {
            LogInfo("Left click: no energy cell in range.");
        }
    }
    lmbPrev = lmbNow;

    bool gNow = glfwGetKey(w, GLFW_KEY_G) == GLFW_PRESS;
    if (gNow && !gPrev) {
        LogInfo("Manual reset requested with G.");
        fadeAlpha = 0.0f;
        fadeState = 0;
        endStateTimer = 0.0f;
        resetGame();
    }
    gPrev = gNow;

    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        LogInfo("Escape pressed. Closing window.");
        glfwSetWindowShouldClose(w, true);
    }

    if (shakeDuration > 0.0f)
        shakeDuration = glm::max(0.0f, shakeDuration - dt);

    updateAsteroids(dt);
    updateGame(dt);

    if (fadeState == 1)
    {
        endStateTimer -= dt;
        if (endStateTimer <= 0.0f)
            fadeState = 2;
    }
    else if (fadeState == 2)
    {
        fadeAlpha += dt * 1.35f;
        if (fadeAlpha >= 1.0f)
        {
            fadeAlpha = 1.0f;
            resetGame();
            fadeState = 3;
        }
    }
    else if (fadeState == 3)
    {
        fadeAlpha -= dt * 1.15f;
        if (fadeAlpha <= 0.0f)
        {
            fadeAlpha = 0.0f;
            fadeState = 0;
        }
    }

    updateWindowTitle();
}

void SceneBasic_Uniform::setPlatformUniforms(const glm::mat4& view,
    const glm::mat4& proj,
    float t)
{
    platformProg.use();
    platformProg.setUniform("View", view);
    platformProg.setUniform("Projection", proj);
    platformProg.setUniform("uLightPos", lightPos);
    platformProg.setUniform("uCamPos", camera.Position);
    platformProg.setUniform("uTime", t);
    platformProg.setUniform("uBHPos", bhPos);
    platformProg.setUniform("uDiffuseTex", 3);
    platformProg.setUniform("uNormalTex", 4);

    platformProg.setUniform("uLightPos2", glm::vec3(0.0f));
    platformProg.setUniform("uLightColor2", glm::vec3(0.0f));
    platformProg.setUniform("uLightStrength2", 0.0f);
}

void SceneBasic_Uniform::renderCell(const EnergyCell& cell,
    const glm::mat4& view,
    const glm::mat4& proj,
    float t)
{
    if (cell.cellScale > 0.01f)
    {
        float yRot = (cell.phase == PickupPhase::Idle) ? t * 0.8f : cell.spinAngle;

        glm::mat4 model =
            glm::translate(glm::mat4(1.f), cell.position) *
            glm::rotate(glm::mat4(1.f), yRot, glm::vec3(0, 1, 0)) *
            glm::scale(glm::mat4(1.f), glm::vec3(0.55f * cell.cellScale));

        platformProg.use();
        platformProg.setUniform("Model", model);
        platformProg.setUniform("uBaseColor", glm::vec3(0.12f, 0.22f, 0.45f));
        platformProg.setUniform("uGlowColor", glm::vec3(0.25f, 0.80f, 1.40f));
        platformProg.setUniform("uGlowStrength", 1.6f);
        platformProg.setUniform("uIsCore", 1);
        platformProg.setUniform("uHasDiffuse", cellModelLoaded && cellModel.diffuseTex ? 1 : 0);
        platformProg.setUniform("uHasNormal", cellModelLoaded && cellModel.normalTex ? 1 : 0);
        platformProg.setUniform("uLightPos2", cell.position);
        platformProg.setUniform("uLightColor2", glm::vec3(0.25f, 0.80f, 1.35f));
        platformProg.setUniform("uLightStrength2", 0.45f);

        if (cellModelLoaded)
            cellModel.draw();
        else {
            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereIdxCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }

    if (cell.phase == PickupPhase::SpinShrink || cell.phase == PickupPhase::Poof)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        platformProg.use();
        platformProg.setUniform("uIsCore", 1);
        platformProg.setUniform("uHasDiffuse", 0);
        platformProg.setUniform("uHasNormal", 0);
        platformProg.setUniform("uLightPos2", cell.position);
        platformProg.setUniform("uLightColor2", glm::vec3(0.25f, 0.80f, 1.50f));
        platformProg.setUniform("uLightStrength2", 0.55f);

        if (cell.phase == PickupPhase::SpinShrink)
        {
            float r = 0.6f + cell.orbScale * 3.0f;
            float a = cell.orbScale * 2.0f;

            glm::mat4 m = glm::translate(glm::mat4(1.f), cell.position) *
                glm::scale(glm::mat4(1.f), glm::vec3(r));
            platformProg.setUniform("Model", m);
            platformProg.setUniform("uBaseColor", glm::vec3(0.0f, 0.1f, 0.4f) * a);
            platformProg.setUniform("uGlowColor", glm::vec3(0.1f, 0.5f, 2.0f) * a);
            platformProg.setUniform("uGlowStrength", 3.0f * a);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereIdxCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
        else
        {
            float p = 1.0f - cell.poofAlpha;

            struct RingDef { float speed; float bright; float offset; };
            RingDef rings[3] = {
                { 1.0f,  1.0f,  0.0f },
                { 0.65f, 0.7f,  0.3f },
                { 0.35f, 0.4f,  0.6f }
            };

            for (auto& rd : rings)
            {
                float rp = glm::clamp(p * rd.speed, 0.f, 1.f);
                float alpha = cell.poofAlpha * rd.bright;
                float r = 0.5f + rp * 9.0f + rd.offset;

                glm::mat4 m = glm::translate(glm::mat4(1.f), cell.position) *
                    glm::scale(glm::mat4(1.f), glm::vec3(r));
                platformProg.setUniform("Model", m);
                platformProg.setUniform("uBaseColor", glm::vec3(0.0f, 0.15f, 0.5f) * alpha);
                platformProg.setUniform("uGlowColor", glm::vec3(0.1f, 0.55f, 2.2f) * alpha);
                platformProg.setUniform("uGlowStrength", 4.0f * alpha);

                glBindVertexArray(sphereVAO);
                glDrawElements(GL_TRIANGLES, sphereIdxCount, GL_UNSIGNED_INT, nullptr);
                glBindVertexArray(0);
            }
        }

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void SceneBasic_Uniform::drawOverlayUI()
{
    auto drawRect = [&](int x, int y, int w, int h, float r, float g, float b)
        {
            if (w <= 0 || h <= 0) return;
            glScissor(x, y, w, h);
            glClearColor(r, g, b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        };

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    int margin = 18;
    int barW = std::max(180, width / 4);
    int barH = 18;

    int dangerX = width - barW - margin;
    int dangerY = height - barH - margin;
    drawRect(dangerX, dangerY, barW, barH, 0.08f, 0.08f, 0.10f);

    int fillW = (int)(barW * glm::clamp(dangerLevel, 0.0f, 1.0f));
    float dr = glm::clamp(dangerLevel * 1.35f, 0.0f, 1.0f);
    float dg = glm::clamp(1.15f - dangerLevel * 1.35f, 0.0f, 1.0f);
    float db = 0.08f;
    drawRect(dangerX, dangerY, fillW, barH, dr, dg, db);

    drawRect(dangerX, dangerY + barH, barW, 2, 0.85f, 0.85f, 0.90f);
    drawRect(dangerX, dangerY - 2, barW, 2, 0.85f, 0.85f, 0.90f);

    int total = (int)energyCells.size();
    int collected = 0;
    for (const auto& c : energyCells) if (c.collected) collected++;

    int boxSize = 22;
    int boxGap = 8;
    int boxesX = margin;
    int boxesY = height - boxSize - margin;

    for (int i = 0; i < total; i++)
    {
        bool filled = i < collected;
        drawRect(boxesX + i * (boxSize + boxGap), boxesY, boxSize, boxSize,
            filled ? 0.15f : 0.08f,
            filled ? 0.75f : 0.08f,
            filled ? 1.00f : 0.12f);
    }

    drawRect(margin, height - 52, std::max(260, width / 3), 12, 0.06f, 0.10f, 0.16f);

    if (fadeAlpha > 0.0f)
    {
        drawRect(0, 0, width, height, fadeAlpha, fadeAlpha, fadeAlpha);
    }

    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
}

void SceneBasic_Uniform::render()
{
    if (width <= 0 || height <= 0) {
        LogWarn("Render skipped because width/height is invalid.");
        return;
    }

    if (height == 0) {
        LogError("Height is zero in render(); would divide by zero in perspective setup.");
        return;
    }

    float t = (float)glfwGetTime();
    glm::mat4 view = camera.getViewMatrix();

    if (shakeDuration > 0.0f)
    {
        float strength = shakeMagnitude * (shakeDuration / 0.45f);
        float shakeX = sinf(t * 97.0f) * strength;
        float shakeY = sinf(t * 73.0f) * strength;
        view = glm::translate(view, glm::vec3(shakeX, shakeY, 0.0f));
    }

    glm::mat4 proj = glm::perspective(glm::radians(48.0f),
        (float)width / (float)height,
        0.1f, 5000.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    skyboxProg.use();
    skyboxProg.setUniform("View", glm::mat4(glm::mat3(view)));
    skyboxProg.setUniform("Projection", proj);
    skyboxProg.setUniform("skybox", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    setPlatformUniforms(view, proj, t);

    platformProg.use();
    platformProg.setUniform("uBaseColor", glm::vec3(0.06f, 0.07f, 0.10f));
    platformProg.setUniform("uGlowColor", glm::vec3(1.0f, 0.45f, 0.08f));
    platformProg.setUniform("uGlowStrength", 1.1f);
    platformProg.setUniform("uIsCore", 0);
    platformProg.setUniform("uHasDiffuse", 0);
    platformProg.setUniform("uHasNormal", 0);
    platformProg.setUniform("uLightPos2", glm::vec3(0.0f));
    platformProg.setUniform("uLightColor2", glm::vec3(0.0f));
    platformProg.setUniform("uLightStrength2", 0.0f);

    glm::mat4 upperModel =
        glm::translate(glm::mat4(1.f), glm::vec3(0, 0.8f, 145.0f)) *
        glm::scale(glm::mat4(1.f), glm::vec3(1.9f, 1.f, 1.9f));
    platformProg.setUniform("Model", upperModel);

    glBindVertexArray(platVAO);
    glDrawArrays(GL_TRIANGLES, 0, platVertCount);

    platformProg.setUniform("uBaseColor", glm::vec3(0.04f, 0.05f, 0.08f));
    platformProg.setUniform("uGlowColor", glm::vec3(1.0f, 0.28f, 0.05f));
    platformProg.setUniform("uGlowStrength", 0.85f);

    glm::mat4 lowerModel =
        glm::translate(glm::mat4(1.f), glm::vec3(0, -1.1f, 145.0f)) *
        glm::scale(glm::mat4(1.f), glm::vec3(2.35f, 1.f, 2.35f));
    platformProg.setUniform("Model", lowerModel);
    glDrawArrays(GL_TRIANGLES, 0, platVertCount);
    glBindVertexArray(0);

    for (size_t i = 0; i < platformPositions.size(); i++)
    {
        const glm::vec3& platPos = platformPositions[i];
        glm::vec3 cellLightPos = platPos + glm::vec3(0.0f, 2.5f, 0.0f);

        platformProg.use();
        platformProg.setUniform("uBaseColor", glm::vec3(0.08f, 0.16f, 0.24f));
        platformProg.setUniform("uGlowColor", glm::vec3(0.15f, 0.55f, 1.00f));
        platformProg.setUniform("uGlowStrength", 0.95f);
        platformProg.setUniform("uIsCore", 0);
        platformProg.setUniform("uHasDiffuse", satellitePlatformTex ? 1 : 0);
        platformProg.setUniform("uHasNormal", 0);
        platformProg.setUniform("uLightPos2", cellLightPos);
        platformProg.setUniform("uLightColor2", glm::vec3(0.22f, 0.80f, 1.35f));
        platformProg.setUniform("uLightStrength2", 1.15f);

        if (satellitePlatformTex)
        {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, satellitePlatformTex);
        }

        glm::mat4 satModel =
            glm::translate(glm::mat4(1.f), platPos) *
            glm::scale(glm::mat4(1.f), glm::vec3(0.75f, 1.f, 0.75f));
        platformProg.setUniform("Model", satModel);

        glBindVertexArray(platVAO);
        glDrawArrays(GL_TRIANGLES, 0, platVertCount);
        glBindVertexArray(0);
    }

    for (const auto& cell : energyCells) {
        if (!cell.collected)
            renderCell(cell, view, proj, t);
    }

    renderAsteroids(view, proj, t);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    diskProg.use();
    glm::mat4 diskModel =
        glm::translate(glm::mat4(1.f), bhPos) *
        glm::rotate(glm::mat4(1.f), glm::radians(18.f), glm::vec3(1, 0, 0)) *
        glm::scale(glm::mat4(1.f), glm::vec3(bhR * 1.12f));

    diskProg.setUniform("Model", diskModel);
    diskProg.setUniform("View", view);
    diskProg.setUniform("Projection", proj);
    diskProg.setUniform("uTime", t);
    diskProg.setUniform("uLightPos", lightPos);
    diskProg.setUniform("uCamPos", camera.Position);
    diskProg.setUniform("uDrawBack", 1);

    glBindVertexArray(diskVAO);
    glDrawArrays(GL_TRIANGLES, 0, diskVertCount);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    prog.use();
    glm::mat4 sphereModel =
        glm::translate(glm::mat4(1.f), bhPos) *
        glm::scale(glm::mat4(1.f), glm::vec3(bhR));

    prog.setUniform("Model", sphereModel);
    prog.setUniform("View", view);
    prog.setUniform("Projection", proj);
    prog.setUniform("uTime", t);
    prog.setUniform("uSkybox", 0);
    prog.setUniform("uBHPos", bhPos);
    prog.setUniform("uCamPos", camera.Position);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIdxCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    diskProg.use();
    diskProg.setUniform("uDrawBack", 0);

    glBindVertexArray(diskVAO);
    glDrawArrays(GL_TRIANGLES, 0, diskVertCount);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    bool horizontal = true, firstPass = true;

    blurProg.use();
    blurProg.setUniform("image", 0);
    blurProg.setUniform("uResolution", glm::vec2((float)width, (float)height));

    for (int i = 0; i < 10; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal ? 1 : 0]);
        blurProg.setUniform("horizontal", horizontal ? 1 : 0);
        blurProg.setUniform("firstPass", firstPass ? 1 : 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
            firstPass ? hdrColorBuf : pingpongColorbuffers[horizontal ? 0 : 1]);

        glBindVertexArray(screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        horizontal = !horizontal;
        firstPass = false;
    }

    GLuint bloomTex = pingpongColorbuffers[horizontal ? 0 : 1];

    if (!hdrColorBuf) LogWarn("hdrColorBuf is 0 during render.");
    if (!bloomTex)    LogWarn("bloomTex is 0 during render.");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hdrProg.use();
    hdrProg.setUniform("sceneTex", 0);
    hdrProg.setUniform("bloomBlurTex", 1);
    hdrProg.setUniform("hdrEnabled", hdrEnabled ? 1 : 0);
    hdrProg.setUniform("bloomEnabled", bloomEnabled ? 1 : 0);
    hdrProg.setUniform("filmMode", filmMode ? 1 : 0);
    hdrProg.setUniform("uTime", t);

    float finalExposure = exposure + dangerLevel * 0.55f;
    if (gameLost) finalExposure = 0.40f;
    if (gameWon)  finalExposure = 2.25f;
    hdrProg.setUniform("exposure", finalExposure);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrColorBuf);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTex);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    drawOverlayUI();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    LogInfo("Resize requested: " + std::to_string(w) + "x" + std::to_string(h));

    if (w <= 0 || h <= 0) {
        LogWarn("Resize ignored because dimensions were invalid.");
        return;
    }

    width = w;
    height = h;

    glViewport(0, 0, width, height);

    if (hdrColorBuf) glDeleteTextures(1, &hdrColorBuf);
    if (rboDepth) glDeleteRenderbuffers(1, &rboDepth);
    if (hdrFBO) glDeleteFramebuffers(1, &hdrFBO);

    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongColorbuffers);

    hdrFBO = 0;
    hdrColorBuf = 0;
    rboDepth = 0;
    pingpongFBO[0] = pingpongFBO[1] = 0;
    pingpongColorbuffers[0] = pingpongColorbuffers[1] = 0;

    setupHDRFramebuffer();
    LogInfo("Resize complete.");
}