// Tiff rotate Win.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Petr Buben

#include <iostream>

#include "opencv2/core/mat.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;


extern  "C" {
#include "tiffvers.h"
}
extern  "C" {
#include "tiffconf.h"
}
extern  "C" {
#include "tiff.h"
}
extern  "C" {
#include "tiffio.h"
}


int main()
{
	string imageName("Qres_K.tif");

//	TIFF* tif = TIFFOpen("Qres_K.tif", "r");

	TIFF* tif = TIFFOpen(imageName.c_str(), "r");

	
	if (!tif) {
		cout << "could not open tiffopen\n";
		return 1;
	}
	
	// Create a matrix to hold the tif image in
	
	Mat image;
		
	// check the tif is open
	if (tif) {
		do {
			unsigned int width, height;
			uint32* raster;

			// get the size of the tiff
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

			uint npixels = width * height; // get the total number of pixels

			raster = (uint32*)_TIFFmalloc(npixels * sizeof(uint32)); // allocate temp memory (must use the tiff library malloc)
			if (raster == NULL) // check the raster's memory was allocaed
			{
				TIFFClose(tif);
				cerr << "Could not allocate memory for raster of TIFF image" << endl;
				return -1;
			}

			// Check the tif read to the raster correctly
			if (!TIFFReadRGBAImage(tif, width, height, raster, 0))
			{
				TIFFClose(tif);
				cerr << "Could not read raster of TIFF image" << endl;
				return -1;
			}

			image = Mat(width, height, CV_8UC4); // create a new matrix of w x h with 8 bits per channel and 4 channels (RGBA)

												 // itterate through all the pixels of the tif
			for (uint x = 0; x < width; x++)
				for (uint y = 0; y < height; y++)
				{
					uint32& TiffPixel = raster[y * width + x]; // read the current pixel of the TIF
					Vec4b& pixel = image.at<Vec4b>(Point(y, x)); // read the current pixel of the matrix
					pixel[0] = TIFFGetB(TiffPixel); // Set the pixel values as BGRA
					pixel[1] = TIFFGetG(TiffPixel);
					pixel[2] = TIFFGetR(TiffPixel);
					pixel[3] = TIFFGetA(TiffPixel);
				}

			_TIFFfree(raster); // release temp memory
							   // Rotate the image 90 degrees couter clockwise
			image = image.t();
			flip(image, image, 0);
			imshow("TIF Image", image); // show the image
			waitKey(0); // wait for anykey before displaying next
		} while (TIFFReadDirectory(tif)); // get the next tif
		TIFFClose(tif); // close the tif file
	}
	
	return 0;
}

