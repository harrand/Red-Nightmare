//
// Created by Harrand on 26/08/2018.
//

#include "audio_manager.hpp"
#include "audio/audio.hpp"

void AudioManager::play_shoot_sound()
{
    tz::audio::play_async(AudioClip{"../res/sounds/shoot.wav"});
}

void AudioManager::play_asteroid_explosion_sound()
{
    tz::audio::play_async(AudioClip{"../res/sounds/explosion.wav"});
}
