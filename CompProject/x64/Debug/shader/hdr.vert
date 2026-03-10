#version 430 core
// hdr.vert
// Simple full-screen quad pass-through vertex shader.
// Used for the HDR tone-mapping pass and the Gaussian blur passes.

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 TexCoords;

void main()
{
    TexCoords   = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
