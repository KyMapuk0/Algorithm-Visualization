#include <SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>


SDL_Color default_color{ 128, 128, 255, 255 };
SDL_Color visit_color{ 128, 255, 128, 255 };
SDL_Color swap_color{ 255, 128, 128, 255 };

void renderRect(SDL_Renderer* renderer, const int x, const int y, const int width, const int height, const int mode, SDL_Color body_color) {
	if (mode) {
		SDL_SetRenderDrawColor(renderer, body_color.r, body_color.g, body_color.b, body_color.a);
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

void renderGraph(SDL_Renderer* renderer, 
				std::vector<SDL_Rect>& data, 
				int first_swap_index,									// -1 if just renderer graph without sorting
				int second_swap_index,									// -1 if just renderer graph without sorting
				int visit_index) {										// -1 if just renderer graph without sorting
	SDL_Rect graph{ 30, 10, 960, 530 };
	const int window_width = 1000;
	const int window_height = 600;
	
	SDL_Rect button{ window_width / 2 - 50,  550, 100, 40 };

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	renderRect(renderer, 30, 10, graph.w, graph.h, 0, {});
	renderRect(renderer, button.x, button.y, button.w, button.h, 0, {});
	renderRect(renderer, button.x + 2, button.y + 2, button.w - 4, button.h - 4, 1, default_color);

	for (int i = 0; i < data.size(); ++i) {
		if (i == first_swap_index) {
			renderRect(renderer, data[i].x, data[i].y, data[i].w, data[i].h, 1, swap_color);
		}
		else if (i == second_swap_index) {
			renderRect(renderer, data[i].x, data[i].y, data[i].w, data[i].h, 1, swap_color);
		}
		else if (i == visit_index) {
			renderRect(renderer, data[i].x, data[i].y, data[i].w, data[i].h, 1, visit_color);
		}
		else {
			renderRect(renderer, data[i].x, data[i].y, data[i].w, data[i].h, 1, default_color);
		}
	}

	SDL_RenderPresent(renderer);
}

void swapRect(SDL_Rect* r1, SDL_Rect* r2) {
	std::swap(r1->y, r2->y);
	std::swap(r1->h, r2->h);
}

void printvec(std::vector<int> vec) {
	for (int i = 0; i < vec.size(); ++i) std::cout << vec[i] << " ";
}

void selsort(SDL_Renderer* renderer, std::vector<SDL_Rect>& data) {
	for (int j = 0; j < data.size(); ++j) {
		int min = data[j].h;
		int minpos = j;
		for (int i = (j + 1); i < data.size(); ++i) {
			renderGraph(renderer, data, j, -1, i);
			//SDL_RenderPresent(renderer);
			if (min > data[i].h) {
				min = data[i].h;
				minpos = i;
			}
			renderGraph(renderer, data, j, minpos, -1);
			//SDL_RenderPresent(renderer);
		}
		//SDL_Delay(100);
		//SDL_RenderPresent(renderer);
		swapRect(&data[minpos], &data[j]);
	}
	for (auto&& value: data) std::cout << value.h << " ";
	std::cout << std::endl;
}

#if 0
template<typename ForwardIt>
void selsortiter(ForwardIt first, ForwardIt last) {
	for (ForwardIt j = first; j != last; ++j) {
		auto min_pos = j;
		for (ForwardIt i = std::next(j); i != last; ++i) {
			if (min_pos->h > i->h) {
				min_pos = i;
			}
		}
		std::iter_swap(j, min_pos);
	}
	std::cout << "During sort: " << std::endl;
	for (auto&& value : std::ranges::subrange(first, last)) {
		std::cout << value.h << " ";
	}
	std::cout << std::endl;
}
#endif

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

std::vector<SDL_Rect> makeRectsFromData() {
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(1, 52);

	std::vector<SDL_Rect> data(85);
	for (int i = 0; i < data.size(); ++i) {
		int index = i;
		data[i].x  = 30 + (++index * 11);
		data[i].h = distribution(generator) * 10;
		data[i].y = 530 + 10 - 2 - data[i].h;
		data[i].w = 10;
	}
	return data;
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

#if 0
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(1, 52);

	std::vector<int> data(85);
	std::vector<std::pair<int, SDL_Rect>> pdata;
	for (auto& value : pdata) {
		value.first = distribution(generator);
		value.second.x = 0;
	}

	std::vector<int> lines_heights(data.size());
	for (int i = 0; i < data.size(); ++i) {
		lines_heights[i] = data[i] * 10;
	}

	std::vector<SDL_Rect> lines_x(data.size()); 
	for (int i = 0; i < data.size(); ++i) {
		int index = i;
		lines_x[i] = { graph.x + (++index * 11), lines_bot_border - data[i] * 10, 10, data[i] * 10 };
		//lines_x[i] = graph.x + (++index * 11);
	}
#endif
	
	initSDL(window, &renderer, window_width, window_height);

	std::vector<SDL_Rect> data = makeRectsFromData();
	renderGraph(renderer, data, -1, -1, -1);
	SDL_RenderPresent(renderer);

#if 0
	SDL_Delay(3000);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	renderRect(renderer, 30, 10, graph.w, graph.h, 0, {});
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	renderRect(renderer, button.x, button.y, button.w, button.h, 0, {});
	renderRect(renderer, button.x + 2, button.y + 2, button.w - 4, button.h - 4, 1, default_color);
	selsort(renderer, data);
	/*selsortiter(data.begin(), data.end());
	std::cout << "After sort: " << std::endl;
	for (auto&& value : data) std::cout << value.h << " ";
	std::cout << std::endl;*/

	for (int i = 0; i < data.size(); ++i) {
		renderRect(renderer, data[i].x, data[i].y, data[i].w, data[i].h, 1, default_color);
	}
	SDL_RenderPresent(renderer);
#endif

	bool running = true;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				selsort(renderer, data);
				renderGraph(renderer, data, -1, -1, -1);
				SDL_RenderPresent(renderer);
#if 0
				for (int i = 0; i < data.size(); ++i) {
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					SDL_RenderClear(renderer);
					renderRect(renderer, 30, 10, graph.w, graph.h, 0, {});
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					renderRect(renderer, button.x, button.y, button.w, button.h, 0, {});
					renderRect(renderer, button.x + 2, button.y + 2, button.w - 4, button.h - 4, 1, default_color);
					renderRect(renderer, data[i].x, data[i].y, data[i].w, data[i].h, 1, visit_color);
					SDL_RenderPresent(renderer);
				}

				std::cout << "Button pressed" << std::endl;
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderClear(renderer);
				renderRect(renderer, 30, 10, graph.w, graph.h, 0, {});
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				renderRect(renderer, button.x, button.y, button.w, button.h, 0, {});
				renderRect(renderer, button.x + 2, button.y + 2, button.w - 4, button.h - 4, 1, default_color);
				selsort(data);
				for (int i = 0; i < data.size(); ++i) {
					renderRect(renderer, data[i].x, data[i].y, data[i].w, data[i].h, 1, default_color);
				}
				SDL_RenderPresent(renderer);
#endif
				break;
			default:
				break;
			}
		}
	}

	SDL_Quit();
	return 0;
}