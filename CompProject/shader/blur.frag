#version 430 core
// blur.frag
// Separable Gaussian blur for the Bloom effect.
// Uses a 5-tap Gaussian kernel (9-sample effectively with symmetric offsets).
// On the first pass, only pixels brighter than 1.0 (HDR) are extracted
// to create the bloom bright-pass — this avoids over-brightening dark areas.
// Subsequent passes blur the result horizontally then vertically (ping-pong).

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform int       horizontal;  // 1 = horizontal pass, 0 = vertical pass
uniform int       firstPass;   // 1 = extract bright pixels (threshold), 0 = blur
uniform vec2      uResolution;

void main()
{
    // Gaussian weights for a 5-tap kernel (normalised to sum ~1)
    float weight[5] = float[](
        0.227027,
        0.194595,
        0.121622,
        0.054054,
        0.016216
    );

    vec2 texelSize = 1.0 / uResolution;
    vec3 result    = vec3(0.0);

    // Centre sample
    vec3 center = texture(image, TexCoords).rgb;
    if (firstPass == 1)
    {
        // Bright-pass threshold: only HDR luminance > 1.0 contributes to bloom
        float brightness = dot(center, vec3(0.2126, 0.7152, 0.0722));
        center = (brightness > 1.0) ? center : vec3(0.0);
    }
    result += center * weight[0];

    // Symmetric off-centre samples
    for (int i = 1; i < 5; ++i)
    {
        vec2 offset = (horizontal == 1)
            ? vec2(texelSize.x * float(i), 0.0)
            : vec2(0.0, texelSize.y * float(i));

        vec3 sampleA = texture(image, TexCoords + offset).rgb;
        vec3 sampleB = texture(image, TexCoords - offset).rgb;

        if (firstPass == 1)
        {
            float brightA = dot(sampleA, vec3(0.2126, 0.7152, 0.0722));
            float brightB = dot(sampleB, vec3(0.2126, 0.7152, 0.0722));
            sampleA = (brightA > 1.0) ? sampleA : vec3(0.0);
            sampleB = (brightB > 1.0) ? sampleB : vec3(0.0);
        }

        result += sampleA * weight[i];
        result += sampleB * weight[i];
    }

    FragColor = vec4(result, 1.0);
}
