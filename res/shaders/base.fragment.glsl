#version 460
#extension GL_ARB_bindless_texture : require

in vec3 f_pos;
in vec2 f_texcoord;
in mat4 f_mvp;
flat in int f_drawid;

#ssbo entity_textures
{
    tz_bindless_sampler entity_texture[#@RNSCENEMAXSIZE];
};

out vec4 frag_colour;

void main()
{
    frag_colour = texture2D(entity_texture[f_drawid], f_texcoord);
}