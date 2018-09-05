//
// Created by Harrand on 04/09/2018.
//

#include "game_sprite.hpp"

PossiblyAnimatedTexture::PossiblyAnimatedTexture(const Texture* unanimated_texture): texture(unanimated_texture){}
PossiblyAnimatedTexture::PossiblyAnimatedTexture(AnimatedTexture* animated_texture): texture(animated_texture){}

void PossiblyAnimatedTexture::update(float delta_time)
{
    if(this->is_animated())
        std::get<AnimatedTexture*>(this->texture)->update(delta_time);
}

bool PossiblyAnimatedTexture::is_animated() const
{
    return std::holds_alternative<AnimatedTexture*>(this->texture);
}

const Texture* PossiblyAnimatedTexture::get_unanimate_texture() const
{
    try
    {
        return std::get<const Texture *>(this->texture);
    }
    catch(const std::bad_variant_access& bva)
    {
        return nullptr;
    }
}

const AnimatedTexture* PossiblyAnimatedTexture::get_animate_texture() const
{
    try
    {
        return std::get<AnimatedTexture*>(this->texture);
    }
    catch(const std::bad_variant_access& bva)
    {
        return nullptr;
    }
}

void PossiblyAnimatedTexture::set_texture(const Texture* unanimated_texture)
{
    this->texture = unanimated_texture;
}

void PossiblyAnimatedTexture::set_animation(AnimatedTexture* animated_texture)
{
    this->texture = animated_texture;
}

GameSprite::GameSprite(Vector2F position, float rotation, Vector2F scale, PossiblyAnimatedTexture texture) : DynamicSprite(1.0f, position, rotation, scale, nullptr), texture(texture)
{
    this->update_texture();
}

void GameSprite::update(float delta_time)
{
    this->texture.update(delta_time);
    DynamicSprite::update(delta_time);
    this->update_texture();
}

void GameSprite::update_texture()
{
    if(this->texture.is_animated())
        DynamicSprite::set_texture(&this->texture.get_animate_texture()->get_frame_texture());
    else
        DynamicSprite::set_texture(this->texture.get_unanimate_texture());
}