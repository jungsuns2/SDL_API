#include <iostream>
#include <SDL.h>

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec wav_spec{};

	Uint8* wav_buffer = nullptr;
	Uint32 wav_length{};

	// 웨이브 파일 로드
	if (SDL_LoadWAV("example.wav", &wav_spec, &wav_buffer, &wav_length) == NULL)
	{
		fprintf(stderr, "Failed to load WAV file: %s\n", SDL_GetError());
		exit(1);
	}

	// 오디오 장치 열기
	SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
	if (dev == 0) 
	{
		fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
		exit(1);
	}

	// 파일을 재생하기 위해 큐에 넣는다.
	int success = SDL_QueueAudio(dev, wav_buffer, wav_length);
	if (success != 0) 
	{
		fprintf(stderr, "Failed to queue audio: %s\n", SDL_GetError());
		exit(1);
	}

	// 연주하거나 멈춘다.
	SDL_PauseAudioDevice(dev, 0);

	SDL_Delay(5000);

	SDL_CloseAudioDevice(dev);
	SDL_FreeWAV(wav_buffer);
	SDL_Quit();

	return 0;
}