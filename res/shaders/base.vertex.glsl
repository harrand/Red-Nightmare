#version 460
#extension GL_ARB_bindless_texture : require
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_texcoord;

#ssbo transforms
{
    mat4 mvp[#@RNSCENEMAXSIZE];
};

#ubo entity_textures
{
    tz_bindless_sampler entity_texture[#@RNSCENEMAXSIZE];
};

out vec3 f_pos;
out vec2 f_texcoord;
out mat4 f_mvp;
out int f_drawid;

void main()
{
    f_mvp = mvp[gl_DrawID];
    gl_Position = f_mvp * vec4(v_pos.x, v_pos.y, v_pos.z, 1.0);
    f_pos = v_pos;
    f_texcoord = v_texcoord;
    f_drawid = gl_DrawID;
}