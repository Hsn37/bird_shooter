#include "SDL.h"			//Main SDL Library
#include "SDL_image.h"		//To display images
#include "SDL_ttf.h"		//To render Text
#include "SDL_mixer.h"		//To play sound effects


class Game
{
public:

	SDL_Texture* texture;
	SDL_Surface* surface;
	Mix_Chunk *effect;
	SDL_Rect rect;

	bool reset;

	Game()
	{}

	void loadImage(const char* path, SDL_Renderer* ren)
	{
		surface = IMG_Load(path);
		texture = SDL_CreateTextureFromSurface(ren, surface);
		SDL_FreeSurface(surface);
	}

	void copyToBuffer(SDL_Renderer* ren)
	{
		SDL_RenderCopy(ren, texture, NULL, &rect);
	}

	void loadFont(SDL_Renderer* ren, TTF_Font* font, const char* text, SDL_Color color)
	{
		surface = TTF_RenderText_Solid(font, text, color);
		texture = SDL_CreateTextureFromSurface(ren, surface);
		SDL_FreeSurface(surface);
	}

	void loadEffect(const char* path)
	{
		effect = Mix_LoadWAV(path);
	}

	void playEffect()
	{
		Mix_PlayChannel(1, effect, 0);
	}
};

