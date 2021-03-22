namespace rn
{
    template<class PixelType>
    void SpriteTextureStorage::add_texture(const char* base_sprite_name, rn::SpriteState state, const tz::gl::Image<PixelType>& image)
    {
        rn::SpriteType type{base_sprite_name, state};
        auto pair_ib = this->tex_storage.emplace(type, tz::gl::Texture{});
        tz::gl::Texture& tex = pair_ib.first->second;
        tex.set_parameters(tz::gl::default_texture_params);
        tex.set_data(image);
        tex.make_terminal();
    }
}