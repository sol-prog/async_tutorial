#include <future>
#include <iostream>
#include <cmath>
#include <string>
#include <chrono>
#include <vector>
#include "ppm.h"
#include "PerlinNoise.h"

//Split "mem" into "parts", e.g. if mem = 10 and parts = 4 you will have: 0,2,4,6,10
//if possible the function will split mem into equal chuncks, if not 
//the last chunck will be slightly larger
std::vector<int> bounds(int parts, int mem) {
    std::vector<int>bnd;
    int delta = mem / parts;
    int reminder = mem % parts;
    int N1 = 0, N2 = 0;
    bnd.push_back(N1);
    for (int i = 0; i < parts; ++i) {
        N2 = N1 + delta;
        if (i == parts - 1)
            N2 += reminder;
        bnd.push_back(N2);
        N1 = N2;
    }
    return bnd;
}


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

		// generate a unique name from id, example 
		std::string tmp, name;
		tmp = std::to_string(id);
		if (int diff = id_width - tmp.length()) {
			for(int i = 0; i < diff; ++i) name += "0";
		}
		name = "img_" + name + tmp + ".ppm";

		// Save the image in a PPM file
		image.write(name);	
}

// Control the numbers of threads used
void drive_make_perlin_noise(int left, int right, int id_width, double delta) {
	for(int id = left; id < right; ++id) {
		double z = (double) id * delta;
		make_perlin_noise(id, id_width, z);
	}
}

int main() {
	std::vector<std::future<void>> futures;
	int frames = 1800;
	int id_width = 4;
	double delta = 1.0 / (double) frames;

    //Number of threads to use
    int parts = 2;

    std::vector<int>bnd = bounds(parts, frames);

	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < parts; ++i) {
		futures.push_back(std::async(drive_make_perlin_noise, bnd[i], bnd[i + 1], id_width, delta));
	}

	for(auto &e : futures) {
	    e.get();
	}	
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;
	std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

	return 0;
}
