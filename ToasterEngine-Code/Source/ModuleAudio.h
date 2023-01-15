#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "..\External\SDL_mixer\include\SDL_mixer.h"

#include <string>

#define DEFAULT_MUSIC_FADE_TIME 60

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	bool Init();
	bool CleanUp();

	// Play a music file from the /audio folder
	bool PlayMusic(std::string path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory from /audio folder
	unsigned int LoadFx(std::string path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	void SetVolume(int vol = 128);

private:

	Mix_Music*				music;
	std::vector<Mix_Chunk*>	fx;
};

#endif // __ModuleAudio_H__