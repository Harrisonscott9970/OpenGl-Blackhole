#version 430 core
// skybox.vert
// Renders a cube-map skybox. The translation component of the view matrix
// is stripped (mat3 cast) so the skybox never moves relative to the camera.
// Uses the w==z trick (pos.xyww) to force depth = 1.0 so the skybox always
// renders behind all other geometry.

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
    TexCoords = aPos;
    // Strip translation from view matrix
    vec4 pos = Projection * View * vec4(aPos, 1.0);
    // Set z = w so NDC depth = 1.0 (far plane) after perspective divide
    gl_Position = pos.xyww;
}
