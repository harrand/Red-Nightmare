//
// Created by Harrand on 26/08/2018.
//

#include "entity_spawner.hpp"

EntitySpawner::EntitySpawner(EntityManager& manager, EntitySpawnerMetrics metrics): manager(manager), metrics(metrics), timer(), random(){}

void EntitySpawner::tick_update(int tps)
{
    // this is called tps times per second.
    // thus, this probability = per_second_probability / tps
    if(this->random.next_int(0, 100) < this->metrics.large_asteroid_spawn_per_tick * 100.0f / tps)
    {
        // spawn a large asteroid
        std::cout << "spawning a large asteroid...\n";
        Asteroid& asteroid = this->manager.spawn_asteroid({}, 0.0f, {Asteroid::large_asteroid_size, Asteroid::large_asteroid_size}, Asteroid::Type::LARGE);
        asteroid.velocity = {this->random.next_float(-100.0f, 100.0f), this->random.next_float(-100.0f, 100.0f)};
    }
    if(this->random.next_int(0, 100) < this->metrics.medium_asteroid_spawn_per_tick * 100.0f / tps)
    {
        // spawn a medium asteroid
    }
    if(this->random.next_int(0, 100) < this->metrics.small_asteroid_spawn_per_tick * 100.0f / tps)
    {
        // spawn a small asteroid
    }
    timer.update();
}