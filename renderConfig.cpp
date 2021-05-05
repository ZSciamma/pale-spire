// This file stores the configuration for the build
//	Changing the constants in this file will alter the 
//	quality and render time of the clip.

// Image resolution: number of pixels in scene
extern const int WINDOW_WIDTH = 640;
extern const int WINDOW_HEIGHT = 480;

// Soft shadows: number of random samples to use for each light
extern const int SHADOW_LIGHT_SAMPLE_NUM = 9;

// Glossy reflections: number of random samples to shoot out of point on glass
//	for the blurry, frosted-glass reflection effect
extern const int GLOSSY_REFLECTION_SAMPLE_NUM = 16;
