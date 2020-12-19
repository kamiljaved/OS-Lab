#include <Timer.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

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



void gridSolver(float *smoothImage, 
					  const int width, const int height) 
{

	//int n_iter = 0;
	while (1) {
		//n_iter++;
		float diff = 0;
		//the loop computes the black cells and red cells in alternate iterations of the while loop
		for ( int y = 1; y < height - 1; y++ ) {
			for ( int x = 1; x < width - 1 ; x += 2 ) {
				float temp = smoothImage[(y * width) + x];  
				smoothImage[(y * width) + x] = 0.2 * (smoothImage[(y * width) + x] + smoothImage[((y-1) * width) + x] + 
												smoothImage[(y * width) + (x-1)] + smoothImage[((y+1) * width) + x] + 
												smoothImage[(y * width) + (x+1)]);
				diff += (float)fabs(smoothImage[(y * width) + x] - temp);
			}
		}

		//the loop computes the black cells and red cells in alternate iterations of the while loop
		for ( int y = 1; y < height - 1; y++ ) {
			for ( int x = 2; x < width - 1 ; x += 2 ) {
				float temp = smoothImage[(y * width) + x];  
				smoothImage[(y * width) + x] = 0.2 * (smoothImage[(y * width) + x] + smoothImage[((y-1) * width) + x] + 
												smoothImage[(y * width) + (x-1)] + smoothImage[((y+1) * width) + x] + 
												smoothImage[(y * width) + (x+1)]);
				diff += (float)fabs(smoothImage[(y * width) + x] - temp);
			}
		}


		if(diff/((float)height*(float)width) < TOL) break; 

	}
	//std::cerr << "No of iterations: " << n_iter << std::endl;


	

}