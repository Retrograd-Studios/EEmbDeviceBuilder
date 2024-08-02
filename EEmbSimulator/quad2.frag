#version 330 core

out vec4 FragColor;
 
in vec2 uv;

uniform vec4 color;
uniform vec4 borderColor;
uniform int figureType;





vec4 drawCircle() 
{
    vec4 rColor = color;
    rColor.a *= 1.f - smoothstep(0.4f, 0.5f, length(vec2(0.5f, 0.5f)-uv));

    vec4 bColor = borderColor;
    float s1 = bColor.a * (1.f - smoothstep(0.4f, 0.5f, length(vec2(0.5f, 0.5f)-uv)));
    float s2 = bColor.a * smoothstep(0.3f, 0.5f, length(vec2(0.5f, 0.5f)-uv));

    //float s1 = bColor.a * (1.f - smoothstep(0.1f, 0.5f, 0.5f-uv.x));
    //s1 += bColor.a * (1.f - smoothstep(0.4f, 0.5f, uv.x-0.5f));
    //float s2 = bColor.a * smoothstep(0.1f, 0.5f, 0.5f-uv.x);
    //s2 += bColor.a * smoothstep(0.3f, 0.5f, uv.x-0.5f);

    bColor.a = s1 * s2;

    rColor += bColor;

    return rColor;
}


vec4 drawRect() 
{
    vec4 rColor = color;
    
    rColor.a *= smoothstep(0.15f, 0.05f, length(0.5f-uv.x));

    vec4 bColor = borderColor;
    float s1 = bColor.a * (1.f - smoothstep(0.4f, 0.5f, length(0.5f-uv.x)));
    float s2 = bColor.a * smoothstep(0.3f, 0.5f, length(0.5f-uv.x));

    bColor.a = s1 * s2;
    rColor += bColor;

    return rColor;
}





void main()
{
    vec4 result = vec4(1.f);

    if (figureType < 2) 
    {
        result = drawRect();
    }
    else 
    {
        result = drawCircle();
    }

    FragColor = result;
}