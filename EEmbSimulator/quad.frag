#version 330 core

out vec4 FragColor;

in vec2 uv;

// texture samplers
uniform sampler2D texture0;
uniform vec4 color;
//uniform float texAlpha;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);
    
    FragColor = texture(texture0, uv) * color;
}