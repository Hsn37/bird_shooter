#include <iostream>
#include <string>
#include <ctime>
#include <stdlib.h>
#include "SDL.h"			//Main SDL Library
#include "SDL_image.h"		//To display images
#include "SDL_ttf.h"		//To render Text
#include "SDL_mixer.h"		//To play sound effects
#include "Game.cpp"			//custom class Game.cpp

using namespace std;

//global variables
bool running = true;		//flag for the execution of the progam	
string sc = "0";			//current score
string lives_left = "2";	//lives remaining


//displays the explosion image. The image enlarges with each iteration.
void explode(Game* kaboom, bool& explosion, int x, int y)
{
	static int p = 0;
	if (p == 0)
	{
		kaboom->rect.h = 50;
		kaboom->rect.w = 70;
		p++;
	}
	else if (p == 1)
	{
		kaboom->rect.h = 62.5;
		kaboom->rect.w = 70 * 1.5;
		p++;
	}
	else if (p == 2)
	{
		kaboom->rect.h = 100;
		kaboom->rect.w = 140;
		p++;
	}
	else if (p >= 3 && p <= 6)
	{
		p++;
	}
	else if (p == 7)
	{
		p = 0;
		explosion = false;
		kaboom->rect.h = 0;
		kaboom->rect.w = 0;
	}

	kaboom->rect.x = x - (kaboom->rect.w / 2);
	kaboom->rect.y = y - (kaboom->rect.h / 2);
}

//checks if a bird has been shot
bool hitCheck(Game* crosshair, Game* bird11, Game* bird12, Game* bird21, Game* bird22, int x, int y)
{
	if (x > bird11->rect.x&& x < bird11->rect.x + 50 && y > bird11->rect.y&& y < bird11->rect.y + 50)
	{
		int n = stoi(sc);
		sc = to_string(++n);
		bird11->reset = true;
		bird11->rect.x = 1200;
		bird12->rect.x = 1200;
		return true;
	}
	else if (x > bird21->rect.x&& x < bird21->rect.x + 50 && y > bird21->rect.y&& y < bird21->rect.y + 50)
	{
		int n = stoi(sc);
		sc = to_string(++n);
		bird21->reset = true;
		bird21->rect.x = 1200;
		bird22->rect.x = 1200;
		return true;
	}
	else
		return false;

}

//updates the score and the lives remaining
void updateScoreAndLives(SDL_Renderer* ren, TTF_Font* font, SDL_Color color, Game* score, Game* lives)
{
	score->loadFont(ren, font, sc.c_str(), color);
	lives->loadFont(ren, font, lives_left.c_str(), color);
}

//updates the y-coordinate of the birds. the y coordinate is set accoriding to sine function, to give the birds a wave motion
int updateYCoord(int base, double angle)
{
	return 50 * sin(angle) + base;
}

//once the birds reach their starting points, gives them a random y coordinate, so that they can start from random starting points.
int randomCoord()
{
	static int previous = 850;
	static int current = 850;
	srand(time(0));
	previous = current;
	int c = 0;
	while (abs(current - previous) < 200 && c < 10)
	{
		current = ((rand() % 6) + 1) * 50;
		c++;
	}
	return current;
}

//updates the images of bird2 on screen
void bird2(Game* bird21, Game* bird22)
{
	static int dec = 6;
	static double angle = 0;
	static int p = 0;
	if (bird21->rect.x > 830 && bird21->reset)
	{
		bird21->rect.y = randomCoord();
		bird22->rect.y = bird21->rect.y;
		bird21->reset = false;
		p = bird21->rect.y;
		dec += 1;
	}
	else if (bird21->rect.x < -100)
	{
		bird21->rect.x = 850;
		bird22->rect.x = 850;
		bird21->reset = true;
		int n = stoi(lives_left);
		n--;
		lives_left = to_string(n);
	}

	bird21->rect.x -= dec;
	bird22->rect.x -= dec;
	bird21->rect.y = updateYCoord(p, angle);
	bird22->rect.y = bird21->rect.y;
	angle += 0.1;
}

//updates the images of bird1 on screen
void bird1(Game* bird11, Game* bird12)
{
	static int dec = 3;
	static int p = 0;
	static double angle = 0;

	if (bird11->rect.x > 830 && bird11->reset)
	{
		bird11->rect.y = randomCoord();
		bird12->rect.y = bird11->rect.y;
		dec += 1;
		p = bird11->rect.y;
		bird11->reset = false;
	}
	else if (bird11->rect.x < -100)
	{
		bird11->rect.x = 850;
		bird12->rect.x = 850;
		bird11->reset = true;
		int n = stoi(lives_left);
		n--;
		lives_left = to_string(n);
	}

	bird11->rect.x -= dec;
	bird12->rect.x -= dec;
	bird11->rect.y = updateYCoord(p, angle);
	bird12->rect.y = bird11->rect.y;
	angle += 0.1;
}

//sets the contents of the render buffer to the screen
void renderToScreen(SDL_Renderer* ren)
{
	SDL_RenderPresent(ren);
}

//copies everything to the render buffer
void updateScreen(SDL_Renderer* ren, Game* background, Game* crosshair, Game* kaboom, Game* bird11, Game* bird12, Game* bird21, Game* bird22, bool bird2flag, Game* score, Game* scoreNum, Game* livesLeft, Game* livesNum)
{
	static unsigned int c = 0;
	static unsigned int k = 0;

	SDL_RenderClear(ren);
	background->copyToBuffer(ren);

	//alternates between the posture of bird1. For some time, it displays the image with wings open. Then it displays the image with closed wings
	if (c % 10 < 5)
		bird11->copyToBuffer(ren);
	else
		bird12->copyToBuffer(ren);

	c++;

	//alternates between the posture of bird2. For some time, it displays the image with wings open. Then it displays the image with closed wings
	if (bird2flag)
	{
		if (k % 10 < 5)
			bird21->copyToBuffer(ren);
		else
			bird22->copyToBuffer(ren);

		k++;
	}

	kaboom->copyToBuffer(ren);
	crosshair->copyToBuffer(ren);
	score->copyToBuffer(ren);
	scoreNum->copyToBuffer(ren);
	livesLeft->copyToBuffer(ren);
	livesNum->copyToBuffer(ren);

	renderToScreen(ren);
}

//handles events
void handleEvents(SDL_Renderer* ren, SDL_Window* window, Game* crosshair, Game* bird11, Game* bird12, Game* bird21, Game* bird22, bool& explosion, int& x, int& y, Game* gunshot)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		//checks if the 'close window' button has been clicked
		if (event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
			running = false;
		}

		//checks mouse motion, to move the cursor
		else if (event.type == SDL_MOUSEMOTION)
		{
			crosshair->rect.x = event.motion.x - 50;
			crosshair->rect.y = event.motion.y - 40;
		}

		//checks if the player has shot the gun
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				//plays the gunshot sound
				Mix_HaltMusic();
				gunshot->playEffect();

				//checks if a bird has been shot
				if (hitCheck(crosshair, bird11, bird12, bird21, bird22, event.button.x, event.button.y))
				{
					explosion = true;
					x = event.button.x;
					y = event.button.y;
				}
			}
		}
	}
}

//clears the render buffer, as it needs to be filled with updated data
void clearRenderer(SDL_Renderer* ren)
{
	SDL_RenderClear(ren);
}

//main execution of the program
int main(int argc, char** argv)
{
	//initializing libraries
	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	TTF_Init();

	//creating window and renderer, and hiding the cursor
	SDL_Window* window = SDL_CreateWindow("Bird Shooting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);
	SDL_Renderer* ren = SDL_CreateRenderer(window, -1, 0);
	SDL_ShowCursor(SDL_DISABLE);

	//Setting up variables for text rendering, where we display the Score and Lives Remaining
	TTF_Font* font = TTF_OpenFont("Raleway.ttf", 35);
	SDL_Color color = { 0,0,0,0 };

	//variables to handle fps
	const int FPS = 60;
	const int FRAME_DELAY = 1000 / FPS;
	Uint32 frameStart;
	int frameTime;

	//flag to display explosion
	bool explosion = false;

	//coordinates of explosion
	int ex_x, ex_y = 0;

	//flag for bird 2, as it will be activated once score is 3
	bool bird2flag = false;

	//creating objects of the Game class, and setting initial coordinates and sizes
	Game* crosshair = new Game();
	Game* background = new Game();
	Game* bird11 = new Game();
	Game* bird12 = new Game();
	Game* bird21 = new Game();
	Game* bird22 = new Game();
	Game* kaboom = new Game();
	Game* score = new Game();
	Game* scoreNum = new Game();
	Game* livesLeft = new Game();
	Game* livesNum = new Game();
	Game* gameover = new Game();
	Game* title = new Game();
	Game* gunshot = new Game();
	Game* startMusic = new Game();
	Game* endMusic = new Game();

	//background image
	background->loadImage("assets/background.png", ren);
	background->rect.h = 600;
	background->rect.w = 800;
	background->rect.x = 0;
	background->rect.y = 0;

	//crosshair 
	crosshair->loadImage("assets/crosshair.png", ren);
	crosshair->rect.h = 80;
	crosshair->rect.w = 100;
	crosshair->rect.x = 600;
	crosshair->rect.y = 260;

	//bird1
	bird11->loadImage("assets/berd2.png", ren);
	bird11->rect.w = 50;
	bird11->rect.h = 50;
	bird11->rect.x = 850;
	bird11->rect.y = 0;
	bird11->reset = true;
	bird12->loadImage("assets/berd.png", ren);
	bird12->rect.w = 50;
	bird12->rect.h = 50;
	bird12->rect.x = 850;
	bird12->rect.y = 0;
	bird12->reset = true;

	//bird2
	bird21->loadImage("assets/berd2.png", ren);
	bird21->rect.w = 0;
	bird21->rect.h = 0;
	bird21->rect.x = 850;
	bird21->rect.y = 400;
	bird21->reset = true;
	bird22->loadImage("assets/berd.png", ren);
	bird22->rect.w = 0;
	bird22->rect.h = 0;
	bird22->rect.x = 850;
	bird22->rect.y = 400;
	bird22->reset = true;

	//initializes the image of explosion
	kaboom->loadImage("assets/kaboom.png", ren);

	//setting the the word 'score'
	score->loadFont(ren, font, "Score: ", color);
	score->rect.x = 0;
	score->rect.y = 0;
	score->rect.h = 30;
	score->rect.w = 100;

	//setting the display of the actual score
	scoreNum->loadFont(ren, font, sc.c_str(), color);
	scoreNum->rect.x = 110;
	scoreNum->rect.y = 0;
	scoreNum->rect.h = 30;
	scoreNum->rect.w = 25;

	//setting the word 'Lives Left'
	livesLeft->loadFont(ren, font, "Lives: ", color);
	livesLeft->rect.x = 0;
	livesLeft->rect.y = 30;
	livesLeft->rect.h = 30;
	livesLeft->rect.w = 100;

	//setting the actual number of lives left
	livesNum->loadFont(ren, font, lives_left.c_str(), color);
	livesNum->rect.x = 110;
	livesNum->rect.y = 30;
	livesNum->rect.h = 30;
	livesNum->rect.w = 25;

	//setting the word 'gameover' to display at the end
	gameover->loadImage("assets/gameover.png", ren);
	gameover->rect.x = 0;
	gameover->rect.y = 0;
	gameover->rect.h = 600;
	gameover->rect.w = 800;

	//setting the title of the game
	title->loadImage("assets/titleofgame.png", ren);
	title->rect.x = 0;
	title->rect.y = 0;
	title->rect.h = 600;
	title->rect.w = 800;

	//loading the sfx
	gunshot->loadEffect("assets/sfx/shotgun.wav");
	startMusic->loadEffect("assets/sfx/start.wav");
	endMusic->loadEffect("assets/sfx/end.wav");

	//pre-game display, which shows the title
	background->copyToBuffer(ren);
	startMusic->playEffect();
	title->copyToBuffer(ren);

	renderToScreen(ren);
	SDL_Delay(4000);

	//main program loop
	while (running == true && stoi(lives_left) >= 0)
	{
		//gets time passed since the start of the program
		frameStart = SDL_GetTicks();

		//checks if any events generated. Polls for events
		handleEvents(ren, window, crosshair, bird11, bird12, bird21, bird22, explosion, ex_x, ex_y, gunshot);
		
		//updates score and lives
		updateScoreAndLives(ren, font, color, scoreNum, livesNum);

		//displays the explosion image, if a bird has been shot
		if (explosion == true)
			explode(kaboom, explosion, ex_x, ex_y);

		//displays bird1 to the screen
		bird1(bird11, bird12);

		//displays bird2, but first checks if the flag is true. The flag is set to true once the score is 3.
		if (bird2flag)
			bird2(bird21, bird22);

		updateScreen(ren, background, crosshair, kaboom, bird11, bird12, bird21, bird22, bird2flag, score, scoreNum, livesLeft, livesNum);

		//adding delay according to time elapsed, so that the framerate is fixed at 60 fps
		//subtracts time at the start of this loop from the current program time. That gives the time for this frame 
		frameTime = SDL_GetTicks() - frameStart;
		if (FRAME_DELAY > frameTime)
			SDL_Delay(FRAME_DELAY - frameTime);

		//set the bird2 flag to true
		if (sc == "3")
		{
			bird2flag = true;
			bird21->rect.w = 50;
			bird21->rect.h = 50;
			bird22->rect.w = 50;
			bird22->rect.h = 50;
		}
	}

	//post game display, where it says 'game over', and displays score
	score->rect.x = 330;
	score->rect.y = 400;
	scoreNum->rect.x = 330 + 110;
	scoreNum->rect.y = 400;
	gameover->copyToBuffer(ren);
	scoreNum->copyToBuffer(ren);
	score->copyToBuffer(ren);
	endMusic->playEffect();

	renderToScreen(ren);
	SDL_Delay(4500);
	

	return 0;
}