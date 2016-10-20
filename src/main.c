#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "main.h"
#include "ppm/header.h"
#include "ppm/p6.h"

/**
 * SQR
 * @param v
 * @return the square of the input double
 */
static inline double sqr(double v) {
	return v*v;
}

/**
 * NORMALIZE
 * @param v
 * @description normalizes a vector
 */
static inline void normalize(double* v) {
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

/**
 * DOT PRODUCT
 * @param vec1
 * @param vec2
 * @return returns a double value representing the dot product of the two
 * provided doubles
 */
static inline double dot_product(double* vec1, double* vec2) {
	return ((vec1[0] * vec2[0]) + (vec1[1] * vec2[1]) + (vec1[2] * vec2[2]));
}

int main(int argc, char *argv[]) {
	// validate that the parameters seem to be correct
	if (validate_parameters(argc, argv)) {
		fprintf(stderr, "Closing Program\n");
		return 1;
	}
	
	// init the json storage structure
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
	
	Object *lights = malloc(sizeof(Light) * 128);
	
	// populate the objects array
	build_scene(objects, lights, root_element);
	
	// set a pointer for moving around the array
	Object *current_object = objects;
	Object camera;
	// find the camera (take the first found)
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
	
	// define the origin
	double center_x = 0;
	double center_y = 0;
	
	// define the height and width of a single pixel in the viewplane
	double pixel_height = height / M;
	double pixel_width = width / N;
	
	// set up a counter for the pixel array
	int pixel_counter = 0;
	int y;
	// iterate over the vertical space from bottom to top
	for (y = M - 1; y >=0; y--) {
		int x;
		// iterate over the horizontal space from left to right
		for (x = 0; x < N; x++) {
			//direction = normalize(pixel - origin)
			double direction[3];
			direction[0] = center_x - (width/2) + pixel_width * (x + 0.5);
			direction[1] = center_y - (height/2) + pixel_height * (y + 0.5);
			direction[2] = 1;
			normalize(direction);
			
			// set the default color to black
			double color_to_write[3];
			color_to_write[0] = 0;
			color_to_write[1] = 0;
			color_to_write[2] = 0;
			
			Object *found_object = malloc(sizeof(Object) * 1);
			found_object->has_data = FALSE;
			
			// define the best t as infinite
			double best_t = INFINITY;
			double t;
			int current_index = 0;
			int best_t_index = -1;
			// set the current object pointer to the root of the objects array
			current_object = objects;
			// make sure that the current object has data before checking it
			while (current_object->has_data != FALSE) {
				// start with t = 0
				t = 0;
				// check the object type and run the intersection function for that type
				switch(current_object->type) {
					case OBJ_SPHERE:
						t = intersect_sphere(origin, direction, current_object);
						break;
					
					case OBJ_PLANE:
						t = intersect_plane(origin, direction, current_object);
						break;
					
					default:
						break;
				}
				// check if a new closest object was found
				if (t > 0 && t < best_t) {
					best_t = t;
					found_object = current_object;
					best_t_index = current_index;
				}
				// increment the object pointer to look at the next object
				current_object++;
				current_index++;
			}
			// get origin of object-light vector
			double object_light_origin[3];
			scale_vector(best_t, direction, object_light_origin);
			add_vectors(object_light_origin, origin, object_light_origin);
			//printf("-----------------------------\n");
			//printf("object_intersect_origin x: %f\n", object_light_origin[0]);
			//printf("object_intersect_origin y: %f\n", object_light_origin[1]);
			//printf("object_intersect_origin z: %f\n\n", object_light_origin[2]);
			int k = 0;
			while (lights[k].has_data == TRUE) {
				//printf("\tLight Location x: %f\n", lights[k].center[0]);
				//printf("\tLight Location y: %f\n", lights[k].center[1]);
				//printf("\tLight Location z: %f\n\n", lights[k].center[2]);
				double object_light_vector[3];
				sub_vectors(lights[k].center, object_light_origin, object_light_vector);
				//printf("\tObject_light_vector x: %f\n", object_light_vector[0]);
				//printf("\tObject_light_vector y: %f\n", object_light_vector[1]);
				//printf("\tObject_light_vector z: %f\n\n", object_light_vector[2]);
				double distance_to_light = sqrt(sqr(object_light_vector[0]) + sqr(object_light_vector[1]) + sqr(object_light_vector[2]));
				normalize(object_light_vector);
				//printf("\tDistance to light: %f\n\n", distance_to_light);
				double shadow_t = INFINITY;
				boolean found_shadow_object = FALSE;
				int l = 0;
				while (objects[l].has_data == TRUE) {
					
					if (l == best_t_index) { l++; continue; }
					// check the object type and run the intersection function for that type
					switch(objects[l].type) {
						case OBJ_SPHERE:
							shadow_t = intersect_sphere(object_light_origin, object_light_vector, &objects[l]);
							if(shadow_t != INFINITY) {
								//printf("shadow intersect value: %f\n\n", shadow_t);
							}
							break;
						
						case OBJ_PLANE:
							shadow_t = intersect_plane(object_light_origin, object_light_vector, &objects[l]);
							if(shadow_t != INFINITY) {
								//printf("shadow intersect value: %f\n\n", shadow_t);
							}
							break;
						
						default:
							
							break;
					}
					if(shadow_t != INFINITY && shadow_t < distance_to_light && shadow_t > 0) {
						found_shadow_object = TRUE;
						//printf("got here\n\n");
						break;
					}
					l++;
				}
				if(found_shadow_object == FALSE) {
					// write the color of the closest object to the current pixel in the array
					switch (found_object->type) {
						case OBJ_PLANE:
							color_to_write[0] = found_object->data.plane.color[0];
							color_to_write[1] = found_object->data.plane.color[1];
							color_to_write[2] = found_object->data.plane.color[2];
							break;
						
						case OBJ_SPHERE:
							color_to_write[0] = found_object->data.sphere.color[0];
							color_to_write[1] = found_object->data.sphere.color[1];
							color_to_write[2] = found_object->data.sphere.color[2];
							break;
						
						default:
							break;
					}
				}
				k++;
			}
			image[pixel_counter].r = color_to_write[0];
			image[pixel_counter].g = color_to_write[1];
			image[pixel_counter].b = color_to_write[2];
			// increment the pixel array
			pixel_counter++;
		}
	}
	// write the P6 file
	FILE* output_file = fopen(argv[4], "w");
	write_header(output_file, "6\0", out_header);
	write_p6(output_file, out_header, image);
	
	fprintf(stderr, "\nProgram completed\n");
	return 0;
}

/**
 * INTERSECT SPHERE
 * @param origin
 * @param direction
 * @param current_object
 * @param color
 * @return a t value that represents the distance from the camera along the
 * provided ray where the sphere was intersected
 * @description checks to see if the provided ray intersects with the provided object and returns
 * the closest distance from where the ray intersected the sphere. It also sets the color (RGB)
 * in the provided double* (values range from 0-1)
 */
double intersect_sphere(double *origin, double *direction, Object *current_object) {
	double t0, t;
	//double a = sqr(direction[0]) + sqr(direction[1]) + sqr(direction[2]);
	double b = 2 * ((direction[0] * (origin[0] - current_object->center[0])) + (direction[1] * (origin[1] - current_object->center[1])) + (direction[2] * (origin[2] - current_object->center[2])));
	double c = sqr(origin[0] - current_object->center[0]) + sqr(origin[1] - current_object->center[1]) + sqr(origin[2] - current_object->center[2]) - sqr(current_object->data.sphere.radius);
	
	double desc = sqr(b) - 4 * c;
	if (desc < 0) return -1;
	
	desc = sqrt(desc);
	
	t0 = (-b - desc) / (2);
	if (t0 > 0) {
		return t0;
	}
	
	t = (-b + desc) / (2);
	if (t > 0) {
		return t;
	}
	
	return -1;
}

/**
 * INTERSECT PLANE
 * @param origin
 * @param direction
 * @param current_object
 * @param color
 * @return a t value that represents the distance from the camera along the
 * provided ray where the plane was intersected
 * @description checks to see if the provided ray intersects with the provided object and returns
 * the closest distance from where the ray intersected the plane. It also sets the color (RGB)
 * in the provided double* (values range from 0-1)
 */
double intersect_plane(double *origin, double *direction, Object *current_object) {
	double t;
	double denominator = dot_product(current_object->data.plane.normal, direction);
	t = dot_product(current_object->center, current_object->data.plane.normal) / denominator;
	return t;
}

/**
 * VALIDATE PARAMETERS
 * @param argc
 * @param argv
 * @return 	0	-	all is well and parameters seem to be valid
 * 			1	-	there aren't enough parameters or the first two aren't numbers
 * @description checks the provided parameters to see if they are what this
 * program requires
 */
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


void scale_vector(double scale, double *vector, double* dest) {
	dest[0] = vector[0] * scale;
	dest[1] = vector[1] * scale;
	dest[2] = vector[2] * scale;
}

void multiply_vectors(double *a, double *b, double *dest) {
	dest[0] = a[0] * b[0];
	dest[1] = a[1] * b[1];
	dest[2] = a[2] * b[2];
}

void add_vectors(double *a, double *b, double *dest) {
	dest[0] = a[0] + b[0];
	dest[1] = a[1] + b[1];
	dest[2] = a[2] + b[2];
}

void sub_vectors(double *a, double *b, double *dest) {
	dest[0] = a[0] - b[0];
	dest[1] = a[1] - b[1];
	dest[2] = a[2] - b[2];
}