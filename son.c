// son.c
#include <SDL.h>
#include "son.h"

static Mix_Music * music = NULL;

int ahInitAudio(const char * musicFile) {
  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "Erreur lors de l'initialisation de SDL_AUDIO : %s\n", SDL_GetError());
    return 0;
  }
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    fprintf(stderr, "Erreur lors de l'initialisation de Mix_OpenAudio : %s\n", Mix_GetError());
    return 0;
  }
  if(!(music = Mix_LoadMUS(musicFile))) {
    fprintf(stderr, "Erreur lors de la lecture du fichier %s : %s\n", musicFile, Mix_GetError());
    return 0;
  }
  return 1;
}

void ahPlayMusic() {
  Mix_PlayMusic(music, -1);
}

void ahClean() {
  if(music) {
    Mix_FreeMusic(music);
    music = NULL;
  }
  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
