// Author: Juraj Vanek
// This shader performs final composition of HDR image with tonemapping and SSAO

out vec4 out_FragColor;
in vec2 fragTexCoord;

//textures
uniform sampler2D render_texture, blur_texture, normal_texture;

// Control exposure with this value
uniform float exposure;
// How much bloom to add
uniform float bloomFactor;
// Max bright
uniform float brightMax;
uniform int show_ao = 0;
uniform int use_ao = 1;

void main()
{
    vec3 color = texture(render_texture, fragTexCoord).rgb;
    vec4 compo_tex = texture(blur_texture, fragTexCoord/2.0);
    vec3 colorBloom = compo_tex.rgb;
    float ssao_term =  compo_tex.a;

    // Add bloom to the image
    color += colorBloom * bloomFactor;
    
    // Perform simple tone-mapping
    //float Y = dot(vec3(0.30, 0.59, 0.11), color);
    float YD = exposure * (exposure/brightMax + 1.0) / (exposure + 1.0);
    color *= YD;

    if(use_ao == 1)
      color *= vec3(ssao_term);

    if(show_ao == 1)
        out_FragColor = vec4(ssao_term);
    else
        out_FragColor.rgb = color;
}
