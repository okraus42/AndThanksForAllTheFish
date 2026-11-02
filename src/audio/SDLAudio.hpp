#pragma once

#include "games/atfatf/AtfatfEvent.hpp"
#include "IAudio.hpp"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <chrono>
#include <string>
#include <memory>

class SDLAudio : public IAudio {
public:
    SDLAudio();
    ~SDLAudio();

    void playEvent(AtfatfEvent event) override;
    void playMusic(const std::string& name, int loops) override;
    void stopMusic() override;

    void setMasterSFXVolume(float vol);
    void setMasterMusicVolume(float vol);

private:
    MIX_Mixer* mixer = nullptr;

    MIX_Audio* pelletAudio = nullptr;
    MIX_Audio* moveAudio   = nullptr;

    MIX_Track* fishTrack = nullptr;
    MIX_Track* moveTrack   = nullptr;

    MIX_Audio* menuMusicAudio = nullptr;
    MIX_Audio* gameMusicAudio = nullptr;
    MIX_Track* musicTrack     = nullptr;

    float masterSFXVolume   = 0.6f;
    float masterMusicVolume = 0.2f;

    // relative volumes
    static constexpr float PELLET_VOLUME = 0.8f;
    static constexpr float MOVE_VOLUME   = 0.3f;

    std::chrono::steady_clock::time_point lastMoveSoundTime{};
    const std::chrono::milliseconds moveSoundCooldown{170};
};