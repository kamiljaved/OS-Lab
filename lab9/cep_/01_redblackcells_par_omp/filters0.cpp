#include <Timer.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <omp.h>

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

void gridSolver(float *smoothImage, const int width, const int height) 
{
	omp_set_num_threads(nTHREADS);

	while (1) {

		float diff = 0;

		#pragma omp parallel for
		//the loop computes the black cells and red cells in alternate iterations of the while loop
		for ( int y = 1; y < height - 1; y++ ) {
			for ( int x = 1; x < width - 1 ; x += 2 ) {
				float temp = smoothImage[(y * width) + x];  
				smoothImage[(y * width) + x] = 0.2 * (smoothImage[(y * width) + x] + smoothImage[((y-1) * width) + x] + 
												smoothImage[(y * width) + (x-1)] + smoothImage[((y+1) * width) + x] + 
												smoothImage[(y * width) + (x+1)]);
				#pragma omp critical
            	{
					diff += (float)fabs(smoothImage[(y * width) + x] - temp);
				}
			}
		}

		{
   			#pragma omp barrier
		}

		#pragma omp parallel for
		//the loop computes the black cells and red cells in alternate iterations of the while loop
		for ( int y = 1; y < height - 1; y++ ) {
			for ( int x = 2; x < width - 1 ; x += 2 ) {
				float temp = smoothImage[(y * width) + x];  
				smoothImage[(y * width) + x] = 0.2 * (smoothImage[(y * width) + x] + smoothImage[((y-1) * width) + x] + 
												smoothImage[(y * width) + (x-1)] + smoothImage[((y+1) * width) + x] + 
												smoothImage[(y * width) + (x+1)]);
				#pragma omp critical
            	{
					diff += (float)fabs(smoothImage[(y * width) + x] - temp);
				}
			}
		}

		// cout << "diff: " << diff/((float)height*(float)width) << endl;
		if(diff/((float)height*(float)width) < TOL) break; 
	}
}