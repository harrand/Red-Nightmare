#include "animated_texture.hpp"
#include "core/debug/assert.hpp"
#include "gl/texture.hpp"
#include <numeric>
#include <utility>

namespace rn
{    
    template<class ImageType>
    bool AnimatedTextureFrame<ImageType>::operator<(const AnimatedTextureFrame& rhs) const
    {
        return this->frame_number < rhs.frame_number;
    }

    template<class ImageType>
    AnimatedTexture<ImageType>::AnimatedTexture(unsigned int fps): fps(fps), time_millis(0.0f), frames(){}

    template<class ImageType>
    void AnimatedTexture<ImageType>::update(float delta_millis)
    {
        this->time_millis += delta_millis;
    }

    template<class ImageType>
    void AnimatedTexture<ImageType>::add_frame(unsigned int frame_number, ImageType handle)
    {
        this->frames.insert({frame_number, std::move(handle)});
    }

    template<class ImageType>
    unsigned int AnimatedTexture<ImageType>::frame_count() const
    {
        unsigned int frame_counter = 0;
        frame_counter = std::accumulate(this->frames.begin(), this->frames.end(), frame_counter, [](unsigned int cur_frame_count, const AnimatedTextureFrame<ImageType>& frame)
        {
            return cur_frame_count + frame.frame_number;
        });
        return frame_counter;
    }

    template<class ImageType>
    const ImageType& AnimatedTexture<ImageType>::get_texture() const
    {
        // frame num = fps * (time_millis / 1000)
        unsigned int frame = static_cast<unsigned int>((this->fps * (this->time_millis / 1000.0f))) % this->frame_count();
        unsigned int fc = 0;
        for(const AnimatedTextureFrame<ImageType>& cur_frame : this->frames)
        {
            if(fc >= frame)
            {
                return cur_frame.handle;
            }
            fc += cur_frame.frame_number;
        }
        return this->frames.begin()->handle;
    }

    template<class ImageType>
    float AnimatedTexture<ImageType>::total_animation_length() const
    {
        // fps = frames / time
        // time = frames / fps
        return this->frame_count() / fps;
    }

    template<typename ImageType>
    AnimatedTexture<tz::gl::Texture> textureify(const AnimatedTexture<ImageType>& animated_image)
    {
        AnimatedTexture<tz::gl::Texture> ret{animated_image.fps};
        for(const AnimatedTextureFrame<ImageType>& img_frame : animated_image.frames)
        {
            unsigned int frame_num = img_frame.frame_number;
            const ImageType& image = img_frame.handle;
            tz::gl::Texture tex;
            tex.set_parameters(tz::gl::default_texture_params);
            tex.set_data(image);
            tex.make_terminal();
            ret.add_frame(frame_num, std::move(tex));
        }
        return ret;
    }
}