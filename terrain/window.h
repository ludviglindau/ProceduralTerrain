#pragma once
#include "../SDL/include/SDL.h"

class Window {
private:
	SDL_Window* window = nullptr;
	SDL_GLContext context;
public:
	~Window();
	void init(const int width, const int height);
	int pollEvent(SDL_Event* e);
	void swapBuffer();
	void setTitle(const char* title);
	void close();
};
