#include "texture.h"

// From M&S, page 244
VEC3 Texture::texture_lookup(float u, float v) const {
	int i = round(u * (float) xRes - 0.5);
	//cout << "i: " << i << endl;
	int j = round(v * (float) yRes - 0.5);
	//cout << "j: " << j << endl;
	int pixel = 3 * ((yRes - j - 1) * xRes + i);	// IS THIS CORRECT?
	//cout << "ppm point: " << pixel << endl;
	//cout << pixel << endl;
	if (pixel >=xRes * yRes * 3 or pixel < 0)									// FIX THIS ERROR!!!!
		return VEC3(0, 1, 0);

	VEC3 colour(pixelValues[pixel], pixelValues[pixel + 1], pixelValues[pixel + 2]);
	//cout << colour << endl; 
	return colour;
}

	// This is the function provided by Professor Kim in assignment 3, in file
	//	HW3_starter/main.cpp, lines 
	void readPPM(const string& filename, int& xRes, int& yRes, float*& values)
	{
	// try to open the file
	FILE *fp;
	fp = fopen(filename.c_str(), "rb");
	if (fp == NULL) {
		cout << " Could not open file \"" << filename.c_str() << "\" for reading." << endl;
		cout << " Make sure you're not trying to read from a weird location or with a " << endl;
		cout << " strange filename. Bailing ... " << endl;
		exit(0);
	}

	// get the dimensions
	unsigned char newline;
	fscanf(fp, "P6\n%d %d\n255%c", &xRes, &yRes, &newline);
	if (newline != '\n') {
		cout << " The header of " << filename.c_str() << " may be improperly formatted." << endl;
		cout << " The program will continue, but you may want to check your input. " << endl;
	}
	int totalCells = xRes * yRes;

	// grab the pixel values
	unsigned char* pixels = new unsigned char[3 * totalCells];
	fread(pixels, 1, totalCells * 3, fp);

	// copy to a nicer data type
	values = new float[3 * totalCells];
	for (int i = 0; i < 3 * totalCells; i++)
	{
		values[i] = (float) pixels[i] / 255.0;
	}

	// clean up
	delete[] pixels;
	fclose(fp);
	//cout << " Read in file " << filename.c_str() << endl;
	}

Texture::Texture(const string& filename, int _xRes, int _yRes)
	: xRes(_xRes), yRes(_yRes) {
	// Load the texture
	readPPM(filename, xRes, yRes, pixelValues);
}
