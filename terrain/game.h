#pragma once
#include "window.h"
#include "graphics.h"
#include <deque>

class Game {
private:
	ivec2 resolution;
	Window window;
	Graphics graphics;
	std::deque<double> dtQueue;
	double dt;
	bool running = false;
	void init();
	void loop();
	void cleanUp();
	void handleEvents();
public:
	void run();
};