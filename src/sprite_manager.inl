namespace rn
{
    template<class PixelType>
    void SpriteTextureStorage::add_texture(const char* base_sprite_name, rn::SpriteState state, const AnimatedTexture<tz::gl::Image<PixelType>>& image)
    {
        rn::SpriteType type{base_sprite_name, state};
        auto pair_ib = this->tex_storage.emplace(type, rn::textureify(image));
        topaz_assertf(pair_ib.second, "rn::SpriteTextureStorage::add_texture(%s, %zu, image): Such a sprite element already exists!", base_sprite_name, static_cast<std::size_t>(state));
        this->sprite_names.insert(std::string{base_sprite_name});
    }
}