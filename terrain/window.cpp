#include "GL/glew.h"
#include <stdexcept>
#include "window.h"

void Window::init(const int width, const int height) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetSwapInterval(0);
#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		char errormsg[200];

		sprintf_s(errormsg, 200, "GLEW error: %s\n", glewGetErrorString(err));
		
		throw std::runtime_error(errormsg);
	}

	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClearDepth(1.0f);

	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("Version %i.%i\n", major, minor);
	int profileMask;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
	if (profileMask = GL_CONTEXT_CORE_PROFILE_BIT) {
		printf("Core profile\n");
	}
	else {
		printf("Compatibility profile\n");
	}
}

Window::~Window() {
	this->close();
}

void Window::close() {
	SDL_DestroyWindow(window);
}

int Window::pollEvent(SDL_Event* e) {
	return SDL_PollEvent(e);
}

void Window::setTitle(const char* title) {
	SDL_SetWindowTitle(window, title);
}

void Window::swapBuffer() {
	SDL_GL_SwapWindow(window);
}
