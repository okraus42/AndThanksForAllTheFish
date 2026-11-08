#pragma once

enum class AtfatfEvent {
    NONE,
    FISH_EATEN,      //sfx_movement_ladder1a  fishAudio
    PLAYER_MOVED,    //sfx_movement_jump1     moveAudio
	TIME_WARNING,    //sfx_sounds_blip1       timeWarningAudio
	PLAYER_HURT,     //sfx_sounds_damage1     hurtAudio
	LEVEL_COMPLETED, //sfx_movement_portal2   levelDoneAudio
	LEVEL_FAILED,    //sfx_sounds_falling10   levelOverAudio
    GAME_STARTED,
	GAME_RESTARTED,
    GAME_OVER
};
