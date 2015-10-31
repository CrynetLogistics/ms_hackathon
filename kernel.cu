#include <iostream>
#include "SDL.h"
#include "stdio.h"
#include "math.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "structures.h"

#undef main
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1000
#define DISPLAY_TIME 30000
#define THREADS_PER_BLOCK 1024
#define NUM_OF_BLOCKS 1000

#define DATA_SET_SIZE 20
#define AXIS_PRESPILL_FACTOR 3
#define UI_BOUNDS 25 //padding
#define LINE_WIDTH 4

void drawPixelRaytracer(SDL_Surface *surface , int x, int y);

int main()
{
    SDL_Window* window = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Surface* outSur = SDL_CreateRGBSurface(0,
                                  SCREEN_WIDTH,
                                  SCREEN_HEIGHT,
                                  32,
                                  0x00FF0000,
                                  0x0000FF00,
                                  0x000000FF,
                                  0xFF000000);


	drawPixelRaytracer(outSur, SCREEN_WIDTH, SCREEN_HEIGHT);
	//SDL_RenderPresent(renderer);
	
	SDL_SaveBMP(outSur, "hello.bmp");

	printf("done");

	//SDL_Delay(DISPLAY_TIME);
	//Destroy window
    SDL_DestroyWindow(window);
    //Quit SDL subsystems
    SDL_Quit();
    return 0;
}


__global__ void simulateIteration(colour_t* d_colourGrid){
	int index = blockIdx.x*blockDim.x+threadIdx.x;
	int x = threadIdx.x;
	int y = blockIdx.x;

	//Y-AXIS
	if(x>=UI_BOUNDS &&  x<SCREEN_WIDTH-UI_BOUNDS && y>=SCREEN_HEIGHT-AXIS_PRESPILL_FACTOR*UI_BOUNDS && y<SCREEN_HEIGHT-AXIS_PRESPILL_FACTOR*UI_BOUNDS+LINE_WIDTH){
		d_colourGrid[index].r=50;
		d_colourGrid[index].g=50;
		d_colourGrid[index].b=50;
	//X-AXIS
	}else if(y>=UI_BOUNDS &&  y<SCREEN_HEIGHT-UI_BOUNDS && x>=AXIS_PRESPILL_FACTOR*UI_BOUNDS && x<AXIS_PRESPILL_FACTOR*UI_BOUNDS+LINE_WIDTH){
		d_colourGrid[index].r=50;
		d_colourGrid[index].g=50;
		d_colourGrid[index].b=50;
	}else{
		d_colourGrid[index].r=200;
		d_colourGrid[index].g=200;
		d_colourGrid[index].b=200;
	}
}

__global__ void drawData(colour_t* d_colourGrid, int* d_dataGrid){
	int idx = threadIdx.x;
	int dataX = d_dataGrid[idx]*4;
	int dataY = d_dataGrid[idx+1];
	int colourIdx = dataX + SCREEN_WIDTH*dataY;

	d_colourGrid[colourIdx].r = 255;
	d_colourGrid[colourIdx].g = 0;
	d_colourGrid[colourIdx].b = 255;
	d_colourGrid[colourIdx+1].r = 255;
	d_colourGrid[colourIdx+1].g = 0;
	d_colourGrid[colourIdx+1].b = 255;
	d_colourGrid[colourIdx+2].r = 255;
	d_colourGrid[colourIdx+2].g = 0;
	d_colourGrid[colourIdx+2].b = 255;
	d_colourGrid[colourIdx+3].r = 255;
	d_colourGrid[colourIdx+3].g = 0;
	d_colourGrid[colourIdx+3].b = 255;
}


void drawPixelRaytracer(SDL_Surface *surface , int x, int y){
	SDL_Rect r;
	r.h = 1;
	r.w = 1;

	colour_t* h_colourGrid = (colour_t*)calloc(x*y, sizeof(colour_t));

	colour_t* d_colourGrid;
	int* d_dataGrid;

	int* h_dataGrid = (int*)malloc(sizeof(int)*DATA_SET_SIZE);
	for(int i=0; i<10; i++){
		h_dataGrid[2*i] = i;
		h_dataGrid[2*i+1] = abs(64 - (float)i*(12+i)/6);
	}

	cudaMalloc((void**) &d_dataGrid, sizeof(int)*DATA_SET_SIZE);
	cudaMalloc((void**) &d_colourGrid, sizeof(colour_t)*x*y);

	cudaMemcpy(d_dataGrid, h_dataGrid, sizeof(int)*DATA_SET_SIZE, cudaMemcpyHostToDevice);
	//cudaMemcpy(d_colourGrid, h_colourGrid, sizeof(colour_t)*x*y, cudaMemcpyHostToDevice);

	//calculateIntensityFromIntersections(thisLocDir, scene, col, squareSize*squareSize);
	//draws base graphics with axes
	simulateIteration<<<NUM_OF_BLOCKS,THREADS_PER_BLOCK>>>(d_colourGrid);

	//draws data
	drawData<<<1,(int)(DATA_SET_SIZE/2)>>>(d_colourGrid, d_dataGrid);

	cudaMemcpy(h_colourGrid, d_colourGrid, sizeof(colour_t)*x*y, cudaMemcpyDeviceToHost);

	cudaFree(d_colourGrid);
	//END OF GPU CALLING CUDA CODE


	for(int i=0;i<x;i++){
		for(int j=0;j<y;j++){
			int index = j*x+i;
			r.x = i;
			r.y = j;

			if(h_colourGrid[index].r<=255 && h_colourGrid[index].g<=255 && h_colourGrid[index].b<=255){
				SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, (int)h_colourGrid[index].r, (int)h_colourGrid[index].g, (int)h_colourGrid[index].b));
			}else{
				//draw bright flourescent pink for regions out of colour range nice one zl
				SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, 255, 0, 255));
			}
		}
	}
	free(h_colourGrid);
	free(h_dataGrid);
}