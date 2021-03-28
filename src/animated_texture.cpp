#include "animated_texture.hpp"

namespace rn
{
    AnimatedTexture<tz::gl::BindlessTextureHandle> handleify(const AnimatedTexture<tz::gl::Texture>& animated_texture)
    {
        AnimatedTexture<tz::gl::BindlessTextureHandle> ret{animated_texture.fps};
        for(const AnimatedTextureFrame<tz::gl::Texture>& tex : animated_texture.frames)
        {
            ret.add_frame(tex.frame_number, tex.handle.get_terminal_handle());
        }
        ret.time_millis = animated_texture.time_millis;
        return ret;
    }
}