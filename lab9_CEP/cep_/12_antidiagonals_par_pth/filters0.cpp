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

pthread_mutex_t mutex;      // for critical section (updating diff)

pthread_barrier_t barrier1;
pthread_barrier_t barrier2;

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

void gridSolver(float *smoothImage, const int width, const int height) 
{

	pthread_t tid[nTHREADS];

	run = true;

	diff = 0;

	g_width = width;
	g_height = height;
	g_smoothImage = smoothImage;

	// init mutex var
    pthread_mutex_init(&mutex,NULL);

	pthread_barrier_init(&barrier1, NULL, nTHREADS);
	pthread_barrier_init(&barrier2, NULL, nTHREADS);

	for (int i=0; i<nTHREADS; i++)
    {
		struct args *args = (struct args*)malloc(sizeof(struct args));
		// args->tid = &tid[i];
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
    
	int tstart = tnum-1;

	while (run) {
		
		// upper half
		for (int x=1; x<g_width-1; x++) {			// for each diagonal in upper half
			for (int i=x-tstart,j=1+tstart; i>0 && j<g_height-1; i-=nTHREADS, j+=nTHREADS) {
				float temp = g_smoothImage[(j * g_width) + i];  
				g_smoothImage[(j * g_width) + i] = 0.2 * (g_smoothImage[(j * g_width) + i] + g_smoothImage[((j-1) * g_width) + i] + 
												g_smoothImage[(j * g_width) + (i-1)] + g_smoothImage[((j+1) * g_width) + i] + 
												g_smoothImage[(j * g_width) + (i+1)]);
				pthread_mutex_lock(&mutex);			// acquire the mutex lock
				diff += (float)fabs(g_smoothImage[(j * g_width) + i] - temp);
				pthread_mutex_unlock(&mutex);		// release the mutex lock
			}
			pthread_barrier_wait(&barrier1);
		}
		// lower half
		for (int y=2; y<g_height-1; y++) {			// for each diagonal in lower half
			for (int i=g_width-2-tstart,j=y+tstart; i>0 && j<g_height-1; i-=nTHREADS, j+=nTHREADS) {
				float temp = g_smoothImage[(j * g_width) + i];  
				g_smoothImage[(j * g_width) + i] = 0.2 * (g_smoothImage[(j * g_width) + i] + g_smoothImage[((j-1) * g_width) + i] + 
												g_smoothImage[(j * g_width) + (i-1)] + g_smoothImage[((j+1) * g_width) + i] + 
												g_smoothImage[(j * g_width) + (i+1)]);
				pthread_mutex_lock(&mutex);			// acquire the mutex lock
				diff += (float)fabs(g_smoothImage[(j * g_width) + i] - temp);
				pthread_mutex_unlock(&mutex);		// release the mutex lock
			}
			pthread_barrier_wait(&barrier1);
		}

		if (tnum == 1) { 
			// cout << "diff " << diff/((float)g_height*(float)g_width) << endl;
			if(diff/((float)g_height*(float)g_width) < TOL) run = false;
			diff = 0;
			pthread_barrier_wait(&barrier2);
		} else {
			pthread_barrier_wait(&barrier2);
		}
	}

	pthread_exit(0);
}