//
// Created by Harrand on 26/08/2018.
//

#ifndef TOPAZASTEROIDS_ENTITY_SPAWNER_HPP
#define TOPAZASTEROIDS_ENTITY_SPAWNER_HPP
#include "entity_manager.hpp"
#include "utility/time.hpp"

struct EntitySpawnerMetrics
{
    float large_asteroid_spawn_per_tick = 0.1f;
    float medium_asteroid_spawn_per_tick = 0.05f;
    float small_asteroid_spawn_per_tick = 0.01f;
};

class EntitySpawner
{
public:
    EntitySpawner(EntityManager& manager, EntitySpawnerMetrics metrics);
    void tick_update(int tps);
private:
    EntityManager& manager;
    EntitySpawnerMetrics metrics;
    Timer timer;
    LocalRandom random;
};


#endif //TOPAZASTEROIDS_ENTITY_SPAWNER_HPP
