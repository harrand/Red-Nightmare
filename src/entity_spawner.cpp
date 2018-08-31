//
// Created by Harrand on 26/08/2018.
//

#include "entity_spawner.hpp"

EntitySpawner::EntitySpawner(EntityManager& manager, EntitySpawnerMetrics metrics): manager(manager), metrics(metrics), timer(), random(){}

void EntitySpawner::tick_update(int tps)
{
    // this is called tps times per second.
    // thus, this probability = per_second_probability / tps
    auto spawn_position = [&]()-> Vector2F
    {
        Vector2F spawn_pos = {};
        if (this->manager.get_screen_wapping_bounds().has_value()) {
            auto bounds = this->manager.get_screen_wapping_bounds().value();
            if (this->random.next_float(0.0f, 1.0f) <= 0.5f)
                spawn_pos.y = this->random.next_int(0, bounds.y);
            else
                spawn_pos.x = this->random.next_int(0, bounds.x);
        }
        return spawn_pos;
    };
    if(this->random.next_int(0, 100) < this->metrics.large_asteroid_spawn_per_tick * 100.0f / tps)
    {
        // spawn a large asteroid
        Asteroid& asteroid = this->manager.spawn_asteroid(spawn_position(), 0.0f, {Asteroid::large_asteroid_size, Asteroid::large_asteroid_size}, Asteroid::Type::LARGE);
        asteroid.velocity = {this->random.next_float(-100.0f, 100.0f), this->random.next_float(-100.0f, 100.0f)};
    }
    if(this->random.next_int(0, 100) < this->metrics.medium_asteroid_spawn_per_tick * 100.0f / tps)
    {
        // spawn a medium asteroid
        Asteroid& asteroid = this->manager.spawn_asteroid(spawn_position(), 0.0f, Vector2F{Asteroid::large_asteroid_size, Asteroid::large_asteroid_size} / 2.0f, Asteroid::Type::MEDIUM);
        asteroid.velocity = {this->random.next_float(-100.0f, 100.0f), this->random.next_float(-100.0f, 100.0f)};
    }
    if(this->random.next_int(0, 100) < this->metrics.small_asteroid_spawn_per_tick * 100.0f / tps)
    {
        // spawn a small asteroid
        Asteroid& asteroid = this->manager.spawn_asteroid(spawn_position(), 0.0f, Vector2F{Asteroid::large_asteroid_size, Asteroid::large_asteroid_size} / 4.0f, Asteroid::Type::SMALL);
        asteroid.velocity = {this->random.next_float(-100.0f, 100.0f), this->random.next_float(-100.0f, 100.0f)};
    }
    timer.update();
}