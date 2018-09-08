//
// Created by Harrand on 04/09/2018.
//

#ifndef REDNIGHTMARE_GAME_SPRITE_HPP
#define REDNIGHTMARE_GAME_SPRITE_HPP
#include "graphics/animated_texture.hpp"
#include "physics/dynamic_sprite.hpp"


class PossiblyAnimatedTexture
{
public:
    PossiblyAnimatedTexture(const Texture* unanimated_texture);
    PossiblyAnimatedTexture(AnimatedTexture* animated_texture);
    void update(float delta_time);
    bool is_animated() const;
    const Texture* get_unanimate_texture() const;
    const AnimatedTexture* get_animate_texture() const;
    void set_texture(const Texture* unanimated_texture);
    void set_animation(AnimatedTexture* animated_texture);
private:
    std::variant<const Texture*, AnimatedTexture*> texture;
};

class GameSprite : public DynamicSprite
{
public:
    GameSprite(Vector2F position, float rotation, Vector2F scale, PossiblyAnimatedTexture texture);
    virtual void update(float delta_time) override;
    virtual void on_collision([[maybe_unused]] GameSprite& other){}
    void set_texture(const Texture* unanimated_texture);
    void set_animation(AnimatedTexture* animated_texture);
private:
    using DynamicSprite::set_texture;
    void update_texture();
    virtual void on_collision(PhysicsObject& other) override;
    PossiblyAnimatedTexture texture;
};


#endif //REDNIGHTMARE_GAME_SPRITE_HPP
