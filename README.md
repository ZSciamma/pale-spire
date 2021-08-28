# pale-spire

This is our final project for the Yale class Computer Graphics, CPSC 478, Spring 2021. The assignment was to build a 10-second movie scene, with a ray tracing engine built from scratch in C++. Our engine can render triangles, rectangles, and cylinders. It has the following features:

* Soft shadows
* Glossy reflections
* Textures
* Cook-Torrance reflectance

You can watch our movie scene here: https://youtu.be/4R8zlZl6tE4

By Matt Udry and Zacharie Sciamma

## Installation

* Clone the repository
* On Mac, install jpgeglib using `brew install jpeglib`
* Install IINA for viewing the movies

## Running

* In the top level directory, call `make` then `./previz`
* Go into the frames directory, then call `make`
* Run `./movieMaker`
* View the output .mov file using IINA (on Mac, right click it then select "Open with IINA")

## Citations

We used code provided by Professor Kim for the Cook-Torrance model. This was the BRDF demo, provided on 19th April 2021, in file BRDFs/chapter6/illumination_models/shaders.js, line 113-176. We edited this code to convert it from GLSL to C++. We used it in the file material.cpp, lines 39-89

## Images

We used the following images as textures for the swimming pool:

swimming_floor_1: https://pixabay.com/photos/water-swimming-pool-wave-pool-1018808/
swimming_wall_1: https://pixabay.com/photos/swimming-pool-pool-tiles-blue-4638912/
swimming_marble_1: https://pixabay.com/photos/marble-texture-white-pattern-2362262/
