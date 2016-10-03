#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "ppm/helpers.h"
#include "json/json_parser.h"
#include "ppm/header.h"
#include "ppm/p6.h"


static inline double sqr(double v) {
	return v*v;
}

static inline void normalize(double* v) {
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

static inline double dot_product(double* vec1, double* vec2) {
	return ((vec1[0] * vec2[0]) + (vec1[1] * vec2[1]) + (vec1[2] * vec2[2]));
}


static inline void subtract_vector(double* vec1, double* vec2, double* destination) {
	destination[0] = vec1[0] - vec2[0];
	destination[1] = vec1[1] - vec2[1];
	destination[2] = vec2[2] - vec2[2];
}

int main(int argc, char *argv[]) {
	if (validate_parameters(argc, argv)) {
		fprintf(stderr, "Closing Program\n");
		return 1;
	}
	
	Json_Element *root_element;
	FILE* json_file = fopen(argv[3], "r");
	
	if (json_file == NULL) {
		fprintf(stderr, "Failed to open json file\n\nClosing Program\n");
		exit(1);
	}
	
	// allocate memory for root element
	root_element = malloc(sizeof(Json_Element) * 1);
	
	int error_check = 0;
	error_check = parse_json(json_file, root_element);
	if (error_check) {
		fprintf(stderr, "Closing Program\n");
		fclose(json_file);
		return 1;
	}
	
	// allocate memory for object array
	Object *objects = malloc(sizeof(Object) * 128);
	
	build_scene(objects, root_element);
	
	Object *current_object = objects;
	Object camera;
	while (current_object != NULL) {
		if(current_object->type == OBJ_CAMERA) {
			camera = *current_object;
			break;
		}
		current_object++;
	}
	
	// set up camera
	double origin[3];
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	double width = camera.data.camera.width;
	double height = camera.data.camera.height;
	// set up view plane
	int M = atoi(argv[2]); // rows
	int N = atoi(argv[1]); // columns
	// set up pixel array
	Pixel *image = malloc(sizeof(Pixel) * M * N);
	// set up header
	Header out_header;
	out_header.height = M;
	out_header.width = N;
	out_header.max_val = 255;
	out_header.magic_number[0] = 'P';
	out_header.magic_number[1] = '6';
	
	
	double center_x = 0;
	double center_y = 0;
	
	double pixel_height = height / M;
	double pixel_width = width / N;
	
	int pixel_counter = 0;
	int y;
	for (y = M - 1; y >=0; y--) {
		int x;
		for (x = 0; x < N; x++) {
			//direction = normalize(pixel - origin)
			double direction[3];
			direction[0] = center_x - (width/2) + pixel_width * (x + 0.5);
			direction[1] = center_y - (height/2) + pixel_height * (y + 0.5);
			direction[2] = 1;
			normalize(direction);
			
			double color_to_write[3];
			color_to_write[0] = 0;
			color_to_write[1] = 0;
			color_to_write[2] = 0;
			
			double current_color[3];
			current_color[0] = 0;
			current_color[1] = 0;
			current_color[2] = 0;
			
			
			double best_t = INFINITY;
			double t;
			current_object = objects;
			int i = 0;
			while (current_object->has_data != FALSE) {
				t = 0;
				switch(current_object->type) {
					case OBJ_SPHERE:
						t = intersect_sphere(origin, direction, current_object, current_color);
						break;
					
					case OBJ_PLANE:
						t = intersect_plane(origin, direction, current_object, current_color);
						break;
					
					case OBJ_CAMERA:
						// do nothing here
						break;
				}
				i++;
				//printf("Loop Number: (%d %d)\nObject Number: %d\n", x, y, i);
				if (t > 0 && t < best_t) {
					best_t = t;
					color_to_write[0] = current_color[0];
					color_to_write[1] = current_color[1];
					color_to_write[2] = current_color[2];
				}
				current_object++;
			}
			
			image[pixel_counter].r = color_to_write[0];
			image[pixel_counter].g = color_to_write[1];
			image[pixel_counter].b = color_to_write[2];
			pixel_counter++;
		}
	}
	
	FILE* output_file = fopen(argv[4], "w");
	write_header(output_file, "6\0", out_header);
	write_p6(output_file, out_header, image);
	
	fprintf(stderr, "\nProgram completed\n");
	return 0;
}

double intersect_sphere(double *origin, double *direction, Object *current_object, double *color) {
	double t0, t;
	double a = sqr(direction[0]) + sqr(direction[1]) + sqr(direction[2]);
	double b = 2 * (direction[0] * (origin[0] - current_object->center[0]) + direction[1] * (origin[1] - current_object->center[1]) + direction[2] * (origin[2] - current_object->center[2]));
	double c = sqr(origin[0] - current_object->center[0]) + sqr(origin[1] - current_object->center[1]) + sqr(origin[2] - current_object->center[2]) - sqr(current_object->data.sphere.radius);
	
	double det = sqr(b) - 4 * a * c;
	if (det < 0) return -1;
	
	t0 = (-b - det) / (2 * a);
	if (t0 > 0) {
		color[0] = current_object->data.sphere.color[0];
		color[1] = current_object->data.sphere.color[1];
		color[2] = current_object->data.sphere.color[2];
		return t0;
	}
	
	t = (-b + det) / (2 * a);
	if (t > 0) {
		color[0] = current_object->data.sphere.color[0];
		color[1] = current_object->data.sphere.color[1];
		color[2] = current_object->data.sphere.color[2];
		return t;
	}
	
	return -1;
}


double intersect_plane(double *origin, double *direction, Object *current_object, double *color) {
	double t;
	double denominator = dot_product(current_object->data.plane.normal, direction);
	t = dot_product(current_object->center, current_object->data.plane.normal) / denominator;
	if (t > 0) {
		color[0] = current_object->data.plane.color[0];
		color[1] = current_object->data.plane.color[1];
		color[2] = current_object->data.plane.color[2];
	}
	return t;
}

int validate_parameters(int argc, char *argv[]) {
	if (argc < 5) {
		fprintf(stderr, "ERROR: Not enough parameters\n\n");
		return 1;
	}
	
	if (!isdigit(argv[1][0])) {
		fprintf(stderr, "ERROR: First parameter must be a whole number > 0 representing the width");
		return 1;
	}
	
	if (!isdigit(argv[2][0])) {
		fprintf(stderr, "ERROR: Second parameter must be a whole number > 0 representing the Height");
		return 1;
	}
	
	return 0;
}