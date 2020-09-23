#include "game.h"
#include "gtc/type_ptr.hpp"

void Game::run() {
	init();
	loop();
	cleanUp();
}

void Game::init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	resolution = ivec2(1280, 720);
	window.init(resolution.x, resolution.y);
	graphics.init(resolution.x, resolution.y);
}

void Game::loop() {
	running = true;
	dt = 1.f / 60.f;
	for (int i = 0; i < 5; ++i) {
		dtQueue.push_back(dt);
	}

	uint tick = 0;
	while (running) {
		dt = (double)(SDL_GetTicks() - tick) * 0.001;
		dtQueue.pop_front();
		dtQueue.push_back(dt);
		tick = SDL_GetTicks();

		handleEvents();
		graphics.draw();
		window.swapBuffer();
		
		double framerate = 0.f;
		for (double prevDeltaTimes : dtQueue) {
			framerate += prevDeltaTimes;
		}
		char title[128] = { 0 };
		sprintf_s(title, 128, "%.0f", 1/(framerate * 0.2f));
		window.setTitle(title);
	}
}

void Game::cleanUp() {
	window.close();
	SDL_Quit();
}

void Game::handleEvents() {
	SDL_Event e;
	bool swappedMouseState = false;
	while (window.pollEvent(&e)) {
		if (e.type == SDL_WINDOWEVENT) {
			if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
				running = false;
			}
		}
		if (e.type == SDL_KEYUP) {
			if (e.key.keysym.sym == SDLK_p) {
				graphics.createShaderProgram();
			}
			if (e.key.keysym.sym == SDLK_n) {
				graphics.createNoiseShaderProgram();
				graphics.generateNoiseTexture(float(SDL_GetTicks() % 1024));
			}
		}
		else if (e.type == SDL_KEYDOWN) {

		}
		if (e.type == SDL_MOUSEBUTTONUP) {
			if (e.button.button == SDL_BUTTON_RIGHT && !swappedMouseState) {
				bool fpsCam = !SDL_GetRelativeMouseMode();
				SDL_SetRelativeMouseMode((SDL_bool)fpsCam);
				SDL_GetRelativeMouseState(nullptr, nullptr);
				swappedMouseState = true;
			}
		}
	}
	
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	double speed = 100.0;
	glm::vec3 move = glm::vec3(0.0);
	if (keyboardState[SDL_SCANCODE_W]) {
		move.z -= float(speed * dt);
	}
	if (keyboardState[SDL_SCANCODE_A]) {
		move.x -= float(speed * dt);
	}
	if (keyboardState[SDL_SCANCODE_S]) {
		move.z += float(speed * dt);
	}
	if (keyboardState[SDL_SCANCODE_D]) {
		move.x += float(speed * dt);
	}
	graphics.camera.relativeMove(move);

	if (SDL_GetRelativeMouseMode()) {
		ivec2 mousePos;
		SDL_GetRelativeMouseState(&mousePos.x, &mousePos.y);
		const glm::ivec2 screenMid = resolution / 2;
		glm::vec2 delta = vec2(mousePos);

		delta *= graphics.camera.sensitivity;
		graphics.camera.orientation += delta;
		graphics.camera.orientation.y = glm::clamp(graphics.camera.orientation.y, -3.14159f / 2.f, 3.14159f / 2.f);
		//wrap around for x axis to prevent rounding errors from accumulation
		graphics.camera.orientation.x = glm::sign(graphics.camera.orientation.x) * glm::mod(glm::abs(graphics.camera.orientation.x), 2.f * 3.14159f);
	}
}
