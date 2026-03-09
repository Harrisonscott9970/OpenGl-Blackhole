#version 430 core
// skybox.frag
// Samples the cube-map texture using the interpolated direction vector.

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}
