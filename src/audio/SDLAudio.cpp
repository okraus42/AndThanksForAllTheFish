#include "audio/SDLAudio.hpp"
#include "logger.hpp"
#include <iostream>

SDLAudio::SDLAudio() {
    // Init audio subsystem
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) == false) {
        std::cerr << "SDL Init Audio failed: " << SDL_GetError() << std::endl;
        return;
    }

    if (MIX_Init() == false) {
        std::cerr << "MIX_Init failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Create mixer bound to default playback device
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer) {
        std::cerr << "MIX_CreateMixerDevice failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Load SFX audio
    fishAudio = MIX_LoadAudio(mixer, "assets/sounds/sfx_movement_ladder1a.wav", true);
    moveAudio   = MIX_LoadAudio(mixer, "assets/sounds/sfx_movement_jump1.wav", true);
	timeWarningAudio = MIX_LoadAudio(mixer, "assets/sounds/sfx_sounds_blip1.wav", true);
	playerHurtAudio = MIX_LoadAudio(mixer, "assets/sounds/sfx_wpn_cannon4.wav", true);
	levelCompletedAudio = MIX_LoadAudio(mixer, "assets/sounds/sfx_movement_portal2.wav", true);
	levelOverAudio = MIX_LoadAudio(mixer, "assets/sounds/sfx_sounds_falling10.wav", true);

    // Create persistent tracks
    if (fishAudio) {
        fishTrack = MIX_CreateTrack(mixer);
		
        LOG_VERBOSE("Created fish track: {}", fishTrack ? "success" : "failure");
        MIX_SetTrackAudio(fishTrack, fishAudio);
        MIX_SetTrackGain(fishTrack, FISH_VOLUME * masterSFXVolume);
    }
    if (moveAudio) {
        moveTrack = MIX_CreateTrack(mixer);
        LOG_VERBOSE("Created move track: {}", moveTrack ? "success" : "failure");
        MIX_SetTrackAudio(moveTrack, moveAudio);
        MIX_SetTrackGain(moveTrack, MOVE_VOLUME * masterSFXVolume);
    }
	if (timeWarningAudio) {
        timeWarningTrack = MIX_CreateTrack(mixer);
        LOG_VERBOSE("Created timeWarning track: {}", timeWarningTrack ? "success" : "failure");
        MIX_SetTrackAudio(timeWarningTrack, timeWarningAudio);
        MIX_SetTrackGain(timeWarningTrack, TIMEWARNING_VOLUME * masterSFXVolume);
    }
	if (playerHurtAudio) {
        playerHurtTrack = MIX_CreateTrack(mixer);
        LOG_VERBOSE("Created playerHurt track: {}", playerHurtTrack ? "success" : "failure");
        MIX_SetTrackAudio(playerHurtTrack, playerHurtAudio);
        MIX_SetTrackGain(playerHurtTrack, PLAYERHURT_VOLUME * masterSFXVolume);
    }
	if (levelCompletedAudio) {
        levelCompletedTrack = MIX_CreateTrack(mixer);
        LOG_VERBOSE("Created levelCompleted track: {}", levelCompletedTrack ? "success" : "failure");
        MIX_SetTrackAudio(levelCompletedTrack, levelCompletedAudio);
        MIX_SetTrackGain(levelCompletedTrack, LEVELCOMPLETED_VOLUME * masterSFXVolume);
    }
	if (levelOverAudio) {
        levelOverTrack = MIX_CreateTrack(mixer);
        LOG_VERBOSE("Created levelOver track: {}", levelOverTrack ? "success" : "failure");
        MIX_SetTrackAudio(levelOverTrack, levelOverAudio);
        MIX_SetTrackGain(levelOverTrack, LEVELOVER_VOLUME * masterSFXVolume);
    }

    // Load music as audio; we assign to track when playing
    menuMusicAudio = MIX_LoadAudio(mixer, "assets/music/title_screen.wav", false);
    gameMusicAudio = MIX_LoadAudio(mixer, "assets/music/level_01.wav", false);

    musicTrack = nullptr;
}
SDLAudio::~SDLAudio() {
    // First, stop all tracks via the mixer
    if (mixer) {
        MIX_StopAllTracks(mixer, 0); // stop everything
    }
    // Now safe to destroy individual tracks
    if (fishTrack) {
        MIX_DestroyTrack(fishTrack);
        fishTrack = nullptr;
    }
    if (moveTrack) {
        MIX_DestroyTrack(moveTrack);
        moveTrack = nullptr;
    }
	if (timeWarningTrack) {
        MIX_DestroyTrack(timeWarningTrack);
        timeWarningTrack = nullptr;
    }
	if (playerHurtTrack) {
        MIX_DestroyTrack(playerHurtTrack);
        playerHurtTrack = nullptr;
    }
	if (levelCompletedTrack) {
        MIX_DestroyTrack(levelCompletedTrack);
        levelCompletedTrack = nullptr;
    }
	if (levelOverTrack) {
        MIX_DestroyTrack(levelOverTrack);
        levelOverTrack = nullptr;
    }
    if (musicTrack) {
        MIX_DestroyTrack(musicTrack);
        musicTrack = nullptr;
    }

    // Destroy audio objects (safe even if assigned to old tracks)
    if (fishAudio) {
        MIX_DestroyAudio(fishAudio);
        fishAudio = nullptr;
    }
    if (moveAudio) {
        MIX_DestroyAudio(moveAudio);
        moveAudio = nullptr;
    }
	if (timeWarningAudio) {
        MIX_DestroyAudio(timeWarningAudio);
        timeWarningAudio = nullptr;
    }
	if (playerHurtAudio) {
        MIX_DestroyAudio(playerHurtAudio);
        playerHurtAudio = nullptr;
    }
	if (levelCompletedAudio) {
        MIX_DestroyAudio(levelCompletedAudio);
        levelCompletedAudio = nullptr;
    }
	if (levelOverAudio) {
        MIX_DestroyAudio(levelOverAudio);
        levelOverAudio = nullptr;
    }
    if (menuMusicAudio) {
        MIX_DestroyAudio(menuMusicAudio);
        menuMusicAudio = nullptr;
    }
    if (gameMusicAudio) {
        MIX_DestroyAudio(gameMusicAudio);
        gameMusicAudio = nullptr;
    }

    // Finally destroy mixer
    if (mixer) {
        MIX_DestroyMixer(mixer);
        mixer = nullptr;
    }
    MIX_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    SDL_Quit();
}

void SDLAudio::playEvent(AtfatfEvent event) {
    auto now = std::chrono::steady_clock::now();
    switch (event) {
        case AtfatfEvent::FISH_EATEN:
            if (fishTrack) MIX_PlayTrack(fishTrack, 0);
            break;
		case AtfatfEvent::TIME_WARNING:
            if (timeWarningTrack) MIX_PlayTrack(timeWarningTrack, 0);
            break;
		case AtfatfEvent::PLAYER_HURT:
            if (playerHurtTrack) MIX_PlayTrack(playerHurtTrack, 0);
            break;
		case AtfatfEvent::LEVEL_COMPLETED:
            if (levelCompletedTrack) MIX_PlayTrack(levelCompletedTrack, 0);
            break;
        case AtfatfEvent::LEVEL_FAILED:
            if (levelOverTrack) MIX_PlayTrack(levelOverTrack, 0);
            break;
		case AtfatfEvent::PLAYER_MOVED:
            if (moveTrack &&
                (now - lastMoveSoundTime >= moveSoundCooldown)) {
                MIX_PlayTrack(moveTrack, 0);
                lastMoveSoundTime = now;
            }
            break;

        case AtfatfEvent::GAME_STARTED:
            playMusic("game", -1);
            break;

        default:
            break;
    }
}

void SDLAudio::playMusic(const std::string& name, int loops) {
    // Stop old music
    if (musicTrack) {
        MIX_StopTrack(musicTrack, 0);
        MIX_DestroyTrack(musicTrack);
        musicTrack = nullptr;
    }

    // Create new music track
    MIX_Audio* audio = nullptr;
    if (name == "menu") audio = menuMusicAudio;
    if (name == "game") audio = gameMusicAudio;

    if (!audio) return;

    musicTrack = MIX_CreateTrack(mixer);
    if (!musicTrack) return;

    MIX_SetTrackAudio(musicTrack, audio);
    MIX_SetTrackGain(musicTrack, masterMusicVolume);
    // MIX_PlayTrack(musicTrack, MIX_PROP_PLAY_LOOPS_NUMBER);
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops); // -1 = infinite looping
    MIX_PlayTrack(musicTrack, props);
    SDL_DestroyProperties(props);
}

void SDLAudio::stopMusic() {
    if (musicTrack) {
        MIX_StopTrack(musicTrack, 0);
        MIX_DestroyTrack(musicTrack);
        musicTrack = nullptr;
    }
}

void SDLAudio::setMasterSFXVolume(float vol) {
    masterSFXVolume = vol;
    if (fishTrack) MIX_SetTrackGain(fishTrack, FISH_VOLUME * masterSFXVolume);
    if (moveTrack)   MIX_SetTrackGain(moveTrack, MOVE_VOLUME * masterSFXVolume);
	if (levelOverTrack)   MIX_SetTrackGain(levelOverTrack, LEVELOVER_VOLUME * masterSFXVolume);
}

void SDLAudio::setMasterMusicVolume(float vol) {
    masterMusicVolume = vol;
    if (musicTrack) MIX_SetTrackGain(musicTrack, masterMusicVolume);
}