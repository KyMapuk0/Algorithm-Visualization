#include <SDL.h>
#include <iostream>
#include <vector>
#include <random>

template<typename T>
void selsort(std::vector<T>& vec) {
	for (int j = 0; j < vec.size(); ++j) {
		int min = vec[j];
		int minpos = j;
		for (int i = (j + 1); i < vec.size(); ++i) {
			if (min > vec[i]) {
				min = vec[i];
				minpos = i;
			}
		}
		int tmp = vec[minpos];
		vec[minpos] = vec[j];
		vec[j] = tmp;
	}
}

template<typename T>
void printvec(std::vector<T> vec) {
	for (int i = 0; i < vec.size(); ++i) std::cout << vec[i] << " ";
}

template<typename T>
void testSort(std::vector<T>& vec) {
	std::cout << "Before: " << std::endl;
	printvec(vec);
	std::cout << std::endl;

	selsort(vec);

	std::cout << "After: " << std::endl;
	printvec(vec);
	std::cout << std::endl;
}

void initSDL(SDL_Window* window, SDL_Renderer** renderer, int w, int h) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) >= 0) {
		window = SDL_CreateWindow("Visualizator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
		if (window) {
			*renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		}
		else {
			std::cout << "From initSDL(), CreateWindow error: " << SDL_GetError();
		}
	}
	else {
		std::cout << "From initSDL(), SDL_Init error: " << SDL_GetError();
	}
}

void renderRect(SDL_Renderer* renderer, const int x, const int y, const int width, const int height, const int mode) {
	if (mode) {
		SDL_SetRenderDrawColor(renderer, 128, 128, 255, 255);
		SDL_Rect inerRect{ x, y, width, height };
		SDL_RenderFillRect(renderer, &inerRect);
		return;
	}
	const int line_width = 2;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect rect{ x, y, line_width, height };
	SDL_RenderFillRect(renderer, &rect);
	rect.x += width - line_width;
	SDL_RenderFillRect(renderer, &rect);
	rect.x = x;
	rect.w = width;
	rect.h = line_width;
	rect.y += height - line_width;
	SDL_RenderFillRect(renderer, &rect);
	rect.y = y;
	SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char* argv[]) {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	const int window_width = 1000;
	const int window_height = 600;
	SDL_Rect graph{ 30, 10, 960, 530 };
	SDL_Rect button { window_width/2 - 50,  550, 100, 40};
	const int graph_line_width = 10;
	const int lines_bot_border = graph.h + graph.y - 2;

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(1, 52);

	std::vector<unsigned> data(85);
	for (unsigned& value : data) {
		value = distribution(generator);
	}

	std::vector<unsigned> lines_x(data.size()); 
	for (int i = 0; i < data.size(); ++i) {
		int index = i;
		lines_x[i] = graph.x + (++index * 11);
	}

	std::vector<unsigned> lines_heights(data.size());
	for (int i = 0; i < data.size(); ++i) {
		lines_heights[i] = data[i] * 10;
	}

	initSDL(window, &renderer, window_width, window_height);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	renderRect(renderer, 30, 10, graph.w, graph.h, 0);

	for (int i = 0; i < data.size(); ++i) {
		renderRect(renderer, lines_x[i], lines_bot_border - lines_heights[i], graph_line_width, lines_heights[i], 1);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	renderRect(renderer, button.x, button.y, button.w, button.h, 0);
	renderRect(renderer, button.x + 2, button.y + 2, button.w - 4, button.h - 4, 1);
	SDL_RenderPresent(renderer);

	bool running = true;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderClear(renderer);
				renderRect(renderer, 30, 10, graph.w, graph.h, 0);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				renderRect(renderer, button.x, button.y, button.w, button.h, 0);
				renderRect(renderer, button.x + 2, button.y + 2, button.w - 4, button.h - 4, 1);
				selsort(data);
				for (int i = 0; i < data.size(); ++i) {
					lines_heights[i] = data[i] * 10;
					renderRect(renderer, lines_x[i], lines_bot_border - lines_heights[i], graph_line_width, lines_heights[i], 1);
				}
				SDL_RenderPresent(renderer);
				break;
			default:
				break;
			}
		}
	}

	SDL_Quit();
	return 0;
}