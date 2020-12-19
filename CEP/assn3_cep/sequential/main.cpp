#include <CImg.h>
#include <Timer.hpp>
#include <iostream>
#include <iomanip>
#include <cstring>

using cimg_library::CImg;
using LOFAR::NSTimer;
using std::cout;
using std::cerr;
using std::endl;
using std::fixed;
using std::setprecision;

// Constants
const bool displayImages = false;
const bool saveAllImages = false;


extern void rgb2gray(float *inputImage, float *grayImage, const int width, const int height);

extern void gridSolver(float *smoothImage, const int width, const int height);


int main(int argc, char *argv[]) 
{
	NSTimer total = NSTimer("total", false, false);

	if ( argc != 2 ) {
		cerr << "Usage: " << argv[0] << " <filename>" << endl;        	cout << fixed << setprecision(6);
		return 1;
	}

	// Load the input image
	CImg< float > inputImage = CImg< float >(argv[1]);
	if ( displayImages ) {
		inputImage.display("Input Image");
	}
	if ( inputImage.spectrum() != 3 ) {
		cerr << "The input must be a color image." << endl;
		return 1;
	}


	// Convert the input image to grayscale 
	CImg<float> grayImage = CImg<float>(inputImage.width(), inputImage.height(), 1, 1);

	rgb2gray(inputImage.data(), grayImage.data(), inputImage.width(), inputImage.height());
        
    cout << fixed << setprecision(6);
        
	
	if ( displayImages ) {
		grayImage.display("Grayscale Image");
	}
	if ( saveAllImages ) {
		grayImage.save("./grayscale.bmp");
	}
	
	
	


	//Blur image
	CImg< float > blurImage = grayImage;


	total.start();

	gridSolver(blurImage.data(), blurImage.width(), blurImage.height());

	total.stop();
        
        
	if ( displayImages ) {
		blurImage.display("Smooth Image");
	}
	blurImage.save("./blur.bmp");

	// Wrap up
	cout << "Execution time of gridSolver: \t" << total.getElapsed() << " seconds." << endl << endl;
	
	return 0;
}

