#include <Timer.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <pthread.h>

#define nCORES 4
#define nTHREADS nCORES

using LOFAR::NSTimer;
using std::cout;
using std::cerr;
using std::endl;
using std::fixed;
using std::setprecision;


void rgb2gray(float *inputImage, float *grayImage, const int width, const int height) 
{

	for ( int y = 0; y < height; y++ ) 
	{
		for ( int x = 0; x < width; x++ ) 
		{
			float grayPix = 0.0f;
			float r = static_cast< float >(inputImage[(y * width) + x]);
			float g = static_cast< float >(inputImage[(width * height) + (y * width) + x]);
			float b = static_cast< float >(inputImage[(2 * width * height) + (y * width) + x]);

			grayPix = (0.3f * r) + (0.59f * g) + (0.11f * b);

			grayImage[(y * width) + x] = grayPix;
		}
	}
	
}




#define TOL 0.1

// global data for threads

pthread_barrier_t barrier1;
pthread_barrier_t barrier2;
pthread_barrier_t barrier3;

void* gridSolverTask(void *input);

// args for threads
struct args {
    // pthread_t* tid;
    int tnum;
};

bool run;

float *g_smoothImage;
int g_width; 
int g_height;

float diff;

#define LEVEL1_DCACHE_LINESIZE 64

struct diffblk {
	float diff;
	char pad[LEVEL1_DCACHE_LINESIZE-4];
};

struct diffblk diffs[nTHREADS];

void gridSolver(float *smoothImage, const int width, const int height) 
{

	pthread_t tid[nTHREADS];

	run = true;

	diff = 0;

	g_width = width;
	g_height = height;
	g_smoothImage = smoothImage;

	pthread_barrier_init(&barrier1, NULL, nTHREADS);
	pthread_barrier_init(&barrier2, NULL, nTHREADS);
	pthread_barrier_init(&barrier3, NULL, nTHREADS);

	for (int i=0; i<nTHREADS; i++)
    {
		struct args *args = (struct args*)malloc(sizeof(struct args));
		args->tnum = i+1;

        // pthread_create(&tid, &attr, function, args);
        pthread_create(&tid[i], NULL, gridSolverTask, (void*)args);
    }

    for (int i=0; i<nTHREADS; i++)
    {
        pthread_join(tid[i], NULL);
    }	

}


void* gridSolverTask(void *input)
{
    int tnum = ((struct args*)input)->tnum;
    if (tnum <= 0) pthread_exit(0);
    
	while (run) {
		
		diffs[tnum-1].diff = 0;

		//the loop computes the black cells and red cells in alternate iterations of the while loop
		for ( int y = tnum; y < g_height - 1; y+=nTHREADS ) {
			for ( int x = 1; x < g_width - 1 ; x += 2 ) {
				float temp = g_smoothImage[(y * g_width) + x];  
				g_smoothImage[(y * g_width) + x] = 0.2 * (g_smoothImage[(y * g_width) + x] + g_smoothImage[((y-1) * g_width) + x] + 
												g_smoothImage[(y * g_width) + (x-1)] + g_smoothImage[((y+1) * g_width) + x] + 
												g_smoothImage[(y * g_width) + (x+1)]);

				diffs[tnum-1].diff += (float)fabs(g_smoothImage[(y * g_width) + x] - temp);
			}
		}

		pthread_barrier_wait(&barrier1);

		//the loop computes the black cells and red cells in alternate iterations of the while loop
		for ( int y = tnum; y < g_height - 1; y+=nTHREADS ) {
			for ( int x = 2; x < g_width - 1 ; x += 2 ) {
				float temp = g_smoothImage[(y * g_width) + x];  
				g_smoothImage[(y * g_width) + x] = 0.2 * (g_smoothImage[(y * g_width) + x] + g_smoothImage[((y-1) * g_width) + x] + 
												g_smoothImage[(y * g_width) + (x-1)] + g_smoothImage[((y+1) * g_width) + x] + 
												g_smoothImage[(y * g_width) + (x+1)]);

				diffs[tnum-1].diff += (float)fabs(g_smoothImage[(y * g_width) + x] - temp);
			}
		}

		pthread_barrier_wait(&barrier2);

		if (tnum == 1) {
			for (int i=0; i<nTHREADS; i++) {
				diff += diffs[i].diff;
			} 
			// cout << "diff " << diff/((float)g_height*(float)g_width) << endl;
			if(diff/((float)g_height*(float)g_width) < TOL) run = false;
			diff = 0;
			pthread_barrier_wait(&barrier3);
		} else {
			pthread_barrier_wait(&barrier3);
		}

	}

	pthread_exit(0);
}


