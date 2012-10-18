#include <future>
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <string>
#include "ppm.h"
#include "PerlinNoise.h"

// Given z as input generate a 2D image from Perlin noise
// id and id_width are parameters use to generate a unique file name
void make_perlin_noise(int id, int id_width, double z) {
	// Define the size of the image
	unsigned int width = 1280, height = 720;

	// Create an empty PPM image
	ppm image(width, height);

	// Create a PerlinNoise object with a random permutation vector generated with seed
	unsigned int seed = 237;
	PerlinNoise pn(seed);

	unsigned int kk = 0;
	// Visit every pixel of the image and assign a color generated with Perlin noise
	for(unsigned int i = 0; i < height; ++i) {     // y
		for(unsigned int j = 0; j < width; ++j) {  // x
			double x = (double)j/((double)width);
			double y = (double)i/((double)height);

			// Wood like structure
			double n = 20 * pn.noise(x, y, z);
			n = n - floor(n);

			// Map the values to the [0, 255] interval, for simplicity we use 
			// tones of grey
			image.r[kk] = floor(255 * n);
			image.g[kk] = floor(255 * n);
			image.b[kk] = floor(255 * n);
			kk++;
		}
	}

	// generate a unique name from id
	std::string tmp, name;
	tmp = std::to_string(id);
	if (int diff = id_width - tmp.length()) {
		for(int i = 0; i < diff; ++i) name += "0";
	}
	name = "img_" + name + tmp + ".ppm";

	// Save the image in a PPM file
	image.write(name);	
}

int main() {
	std::vector<std::future<void>> futures;
	int frames = 1800;
	int id_width = 4;
	double delta = 1.0 / (double) frames;


	auto start = std::chrono::steady_clock::now();
	for(int id = 0; id <= frames; ++id) {
		double z = (double) id * delta;
		futures.push_back(std::async(make_perlin_noise, id, id_width, z));
	}

	for(auto &e : futures) {
	    e.get();
	}	
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;
	std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

	return 0;
}
