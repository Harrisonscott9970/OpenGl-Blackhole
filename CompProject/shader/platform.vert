#version 430 core
// platform.vert
// Vertex shader used for:  platforms, energy cells, and asteroids.
// Outputs TBN matrix for normal-map support (tangent-space lighting).

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
out mat3 vTBN;

void main()
{
    vec4 world  = Model * vec4(aPos, 1.0);
    vWorldPos   = world.xyz;

    // Build normal matrix (handles non-uniform scale correctly)
    mat3 normalMat = mat3(transpose(inverse(Model)));

    vec3 T = normalize(normalMat * aTangent);
    vec3 N = normalize(normalMat * aNormal);
    // Re-orthogonalise T with respect to N (Gram-Schmidt)
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vTBN    = mat3(T, B, N);
    vNormal = N;
    vUV     = aUV;

    gl_Position = Projection * View * world;
}
