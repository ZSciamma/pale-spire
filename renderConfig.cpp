// This file stores the configuration for the build
//	Changing the constants in this file will alter the 
//	quality and render time of the clip.

// Image resolution: number of pixels in scene
extern const int WINDOW_WIDTH = 1600;
extern const int WINDOW_HEIGHT = 1200;

// Distributed ray tracing: sqrt of number of bins to use in stratified sampling.
//	Setting it to n will make n^2 bins. For each pixel, the ray tracer generates
//	a ray in every bin. (so using 2 power will generate 4 rays for each pixel,
//	which should already be pretty nice for avoiding jaggies).
//	1 is weird. You'll expect it to be non-distributed but it still uses random sampling.
extern const int STRATIFIED_SAMPLING_ROOT = 2;	

// Soft shadows: number of random samples to use for each light
extern const int SHADOW_LIGHT_SAMPLE_NUM = 4;		// 9 is pretty nice

// Glossy reflections: number of random samples to shoot out of point on glass
//	for the blurry, frosted-glass reflection effect
extern const int GLOSSY_REFLECTION_SAMPLE_NUM = 9;		// 16 is pretty nice
