#version 430 core
// disk.vert
// Passes world-space and local-space positions to the fragment shader
// for the procedural accretion disk rendering.

layout (location = 0) in vec3 position;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 vWorldPos;
out vec3 vLocalPos;

void main()
{
    vLocalPos   = position;
    vec4 world  = Model * vec4(position, 1.0);
    vWorldPos   = world.xyz;
    gl_Position = Projection * View * world;
}
