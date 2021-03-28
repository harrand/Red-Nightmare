#ifndef REDNIGHTMARE_ANIMATED_TEXTURE_HPP
#define REDNIGHTMARE_ANIMATED_TEXTURE_HPP
#include "gl/image.hpp"
#include "gl/texture.hpp"
#include <set>

namespace rn
{
    template<class ImageType>
    struct AnimatedTextureFrame
    {
        unsigned int frame_number;
        ImageType handle;

        bool operator<(const AnimatedTextureFrame<ImageType>& rhs) const;
    };

    template<class ImageType>
    class AnimatedTexture
    {
    public:
        AnimatedTexture(unsigned int fps = 0);
        void update(float delta_millis);
        void add_frame(unsigned int frame_number, ImageType handle);
        unsigned int frame_count() const;
        const ImageType& get_texture() const;
        float total_animation_length() const;

        unsigned int fps;
        float time_millis;
        std::multiset<AnimatedTextureFrame<ImageType>> frames;
    };

    template<typename ImageType>
    AnimatedTexture<tz::gl::Texture> textureify(const AnimatedTexture<ImageType>& animated_image);
    AnimatedTexture<tz::gl::BindlessTextureHandle> handleify(const AnimatedTexture<tz::gl::Texture>& animated_texture);
}

#include "animated_texture.inl"
#endif // REDNIGHTMARE_ANIMATED_TEXTURE_HPP