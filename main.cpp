#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

struct ComplexNumber
{
	double real;
	double imaginary;

	inline ComplexNumber operator+(double num)
	{
		ComplexNumber result;
		result.real = this->real+num;
		result.imaginary = this->imaginary;

		return result;
	}

	inline ComplexNumber operator+(ComplexNumber& c)
	{
		ComplexNumber result;
		result.real = this->real+c.real;
		result.imaginary = this->imaginary+c.imaginary;

		return result;
	}

	inline ComplexNumber operator*(double num)
	{
		ComplexNumber result;
		result.real = this->real*num;
		result.imaginary = this->imaginary*num;

		return result;
	}

	inline ComplexNumber operator*(ComplexNumber& c)
	{
		ComplexNumber result;
		result.real = (this->real*c.real - this->imaginary*c.imaginary);
		result.imaginary = (this->real*c.imaginary + this->imaginary*this->real);

		return result;
	}	
};

inline ComplexNumber maldelFunc(ComplexNumber z, ComplexNumber c)
{
	return z*z + c;
}

inline double map(double value, double min, double max, double resultMin, double resultMax)
{
	double oldRange = max - min;
	double newRange = resultMax - resultMin;

	return (((value - min) * newRange) / oldRange) + resultMin;
}

void calculateMandelset(double* mandelSet, int width, int height, double maxiteration, double zoom=1, double posX=-2, double posY=-2)
{
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			ComplexNumber c;
			c.real = map(x, 0, width-1, -2/zoom - posX, 2/zoom -posX);
			c.imaginary = map(y, 0, height-1, -2/zoom + posY, 2/zoom + posY);

			ComplexNumber z;
			z.real = 0;
			z.imaginary = 0;

			int n = 0;
			while(n < maxiteration && z.real*z.real + z.imaginary*z.imaginary < 2*2)
			{
				z = maldelFunc(z, c);
				n++;
			}
			if(n == maxiteration)
			 	mandelSet[y*width + x] = 0;
			else
				mandelSet[y*width + x] = map(n, 0, maxiteration, 0, 1);
		}
	}
}

void renderMandelSet(double* mandelSet, SDL_Surface* surface, int width, int height)
{
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			double bright = mandelSet[y*width + x];
			int color = (int)(255*bright);

			Uint8* curPixel = (Uint8*)surface->pixels + y*surface->pitch + x*surface->format->BytesPerPixel;
			curPixel[0] = color; 	//R
			curPixel[1] = color; 	//G
			curPixel[2] = color;	//B
			curPixel[3] = 0xFF;		//A
		}
	}
}

int main(int argc, char *argv[])
{
	int screenWidth = 400;
	int screenHeight = 400;
	int maxiteration = argc > 1 ? atoi(argv[1]) : 256;
	double zoom = argc > 2 ? atoi(argv[2]) : 1;
	double offsetX = 0;
	double offsetY = 0;
	double* mandelSet = new double[screenHeight*screenWidth];

	SDL_Window* window;
	SDL_Surface* screen;
	SDL_Surface* buffer;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("Could not initialize SDL Video System%s\n", SDL_GetError());

	window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
	screen = SDL_GetWindowSurface(window);
	buffer = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, 0);

	calculateMandelset(mandelSet, screenWidth, screenHeight, maxiteration, zoom, offsetX, offsetY);
	renderMandelSet(mandelSet, buffer, screenWidth, screenHeight);

	bool running = true;

	while(running)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
				running = false;
			if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_KP_PLUS:
					zoom*=2;
					break;
					case SDLK_KP_MINUS:
					zoom/=2;
					break;
					case SDLK_UP:
					offsetY-=0.5f/zoom;
					break;
					case SDLK_DOWN:
					offsetY+=0.5f/zoom;
					break;
					case SDLK_LEFT:
					offsetX+=0.5f/zoom;
					break;
					case SDLK_RIGHT:
					offsetX-=0.5f/zoom;
					break;

				}
				calculateMandelset(mandelSet, screenWidth, screenHeight, maxiteration, zoom, offsetX, offsetY);
				renderMandelSet(mandelSet, buffer, screenWidth, screenHeight);
			}
		}

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_BlitSurface(buffer, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(1);
	}


	SDL_Quit();
	SDL_DestroyWindow(window);
	SDL_FreeSurface(buffer);
	window = NULL;
	buffer = NULL;

	return 0;
}