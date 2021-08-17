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
	while (1) {

		float diff = 0;
		
		// upper half
		for (int x=1; x<width-1; x++) {
			for (int i=x,j=1; i>0 && j<height-1; i--, j++) {
				float temp = smoothImage[(j * width) + i];  
				smoothImage[(j * width) + i] = 0.2 * (smoothImage[(j * width) + i] + smoothImage[((j-1) * width) + i] + 
												smoothImage[(j * width) + (i-1)] + smoothImage[((j+1) * width) + i] + 
												smoothImage[(j * width) + (i+1)]);
				diff += (float)fabs(smoothImage[(j * width) + i] - temp);
			}
		}
		// lower half
		for (int y=2; y<height-1; y++) {
			for (int i=width-2,j=y; i>0 && j<height-1; i--, j++) {
				float temp = smoothImage[(j * width) + i];  
				smoothImage[(j * width) + i] = 0.2 * (smoothImage[(j * width) + i] + smoothImage[((j-1) * width) + i] + 
												smoothImage[(j * width) + (i-1)] + smoothImage[((j+1) * width) + i] + 
												smoothImage[(j * width) + (i+1)]);
				diff += (float)fabs(smoothImage[(j * width) + i] - temp);
			}
		}

		// cout << "diff " << diff/((float)height*(float)width) << endl;
		if(diff/((float)height*(float)width) < TOL) break; 
	}
}


// NOTE
//
// for ( ...; i>0, j<height-1; ... )
//
// In the C and C++ programming languages, the comma operator (represented by the token ,) 
// is a binary operator that evaluates its first operand and discards the result, and then 
// evaluates the second operand and returns this value (and type).

