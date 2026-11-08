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

	// FISH_EATEN,      //sfx_movement_ladder1a  fishAudio
    // PLAYER_MOVED,    //sfx_movement_jump1     moveAudio
	// TIME_WARNING,    //sfx_sounds_blip1       timeWarningAudio
	// PLAYER_HURT,     //sfx_sounds_damage1     hurtAudio
	// LEVEL_COMPLETED, //sfx_movement_portal2   levelDoneAudio
	// LEVEL_FAILED,    //sfx_sounds_falling10   levelOverAudio

    static constexpr float FISH_VOLUME = 0.5f;
    MIX_Audio* fishAudio = nullptr;
    MIX_Track* fishTrack = nullptr;

    static constexpr float MOVE_VOLUME   = 0.125f;
    MIX_Audio* moveAudio   = nullptr;
    MIX_Track* moveTrack   = nullptr;

	static constexpr float TIMEWARNING_VOLUME   = 0.5f;
	MIX_Audio* timeWarningAudio = nullptr;
	MIX_Track* timeWarningTrack   = nullptr;

	static constexpr float PLAYERHURT_VOLUME   = 0.5f;
	MIX_Audio* playerHurtAudio = nullptr;
	MIX_Track* playerHurtTrack   = nullptr;

	static constexpr float LEVELCOMPLETED_VOLUME   = 0.5f;
	MIX_Audio* levelCompletedAudio = nullptr;
	MIX_Track* levelCompletedTrack   = nullptr;
	
	static constexpr float LEVELOVER_VOLUME   = 0.5f;
	MIX_Audio* levelOverAudio = nullptr;
	MIX_Track* levelOverTrack   = nullptr;

    MIX_Audio* menuMusicAudio = nullptr;
    MIX_Audio* gameMusicAudio = nullptr;
    MIX_Track* musicTrack     = nullptr;

    float masterSFXVolume   = 0.6f;
    float masterMusicVolume = 0.2f;

    // relative volumes

    std::chrono::steady_clock::time_point lastMoveSoundTime{};
    const std::chrono::milliseconds moveSoundCooldown{170};
};