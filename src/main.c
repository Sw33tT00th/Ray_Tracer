#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "main.h"
#include "ppm/header.h"
#include "ppm/p6.h"
#include "scene.h"

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
			int k = 0;
			while (lights[k].has_data == TRUE) {
				double object_light_vector[3];
				sub_vectors(lights[k].center, object_light_origin, object_light_vector);
				double distance_to_light = sqrt(sqr(object_light_vector[0]) + sqr(object_light_vector[1]) + sqr(object_light_vector[2]));
				normalize(object_light_vector);
				double shadow_t = INFINITY;
				boolean found_shadow_object = FALSE;
				int l = 0;
				while (objects[l].has_data == TRUE) {
					
					if (l == best_t_index) { l++; continue; }
					// check the object type and run the intersection function for that type
					switch(objects[l].type) {
						case OBJ_SPHERE:
							shadow_t = intersect_sphere(object_light_origin, object_light_vector, &objects[l]);
							break;
						
						case OBJ_PLANE:
							shadow_t = intersect_plane(object_light_origin, object_light_vector, &objects[l]);\
							break;
						
						default:
							
							break;
					}
					if(shadow_t != INFINITY && shadow_t < distance_to_light && shadow_t > 0) {
						found_shadow_object = TRUE;
						break;
					}
					l++;
				}
				if(found_shadow_object == FALSE) {
					// write the color of the closest object to the current pixel in the array
					double normal[3];
					double reflection_vector[3];
					
					double diffuse_color[3];
					double specular_color[3] = {1, 1, 1};
					
					switch (found_object->type) {
						case OBJ_PLANE:
							copy_vector(found_object->data.plane.normal, normal);
							
							copy_vector(found_object->data.plane.color, diffuse_color);
							break;
						
						case OBJ_SPHERE:
							sub_vectors(object_light_origin, found_object->center, normal);
							
							copy_vector(found_object->data.sphere.color, diffuse_color);
							copy_vector(found_object->data.sphere.specular_color, specular_color);
							break;
						
						default:
							break;
					}
					reflect_vector(object_light_vector, normal, reflection_vector);
					
					double diffuse_out[3] = {0, 0, 0};
					double specular_out[3] = {0, 0, 0};
					
					normalize(reflection_vector);
					
					diffuse_reflection(normal, object_light_vector, lights[k].data.light.color, diffuse_color, diffuse_out);
					specular_highlight(normal, object_light_vector, reflection_vector, direction, specular_color, lights[k].data.light.color, specular_out);
					
					double angular_out;
					double radial_out;
					
					double reverse_direction[3];
					scale_vector(-1, lights[k].data.light.direction, reverse_direction);
					
					angular_out = fang(lights[k].data.light.angularA0, lights[k].data.light.theta, reverse_direction, object_light_vector);
					radial_out = frad(lights[k].data.light.radialA0, lights[k].data.light.radialA1, lights[k].data.light.radialA2, distance_to_light);
					
					color_to_write[0] = angular_out * radial_out * (diffuse_out[0] + specular_out[0]);
					color_to_write[1] = angular_out * radial_out * (diffuse_out[1] + specular_out[1]);
					color_to_write[2] = angular_out * radial_out * (diffuse_out[2] + specular_out[2]);
				}
				k++;
			}
			
			color_to_write[0] = clamp(color_to_write[0], 0, 1);
			color_to_write[1] = clamp(color_to_write[1], 0, 1);
			color_to_write[2] = clamp(color_to_write[2], 0, 1);
			
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


void specular_highlight(double *normal, double *object_light_vector, double *reflected_vector, double *direction, double *specular_color, double *light_color, double *color) {
	double scalar1, scalar2, scalar3 = 0;
	
	scalar1 = dot_product(normal, object_light_vector);
	scalar2 = dot_product(direction, reflected_vector);
	
	
	if ((scalar1 > 0) && (scalar2 > 0)) {
		scalar3 = pow(scalar2, 20);
		color[0] = scalar3 * specular_color[0] * light_color[0];
		color[1] = scalar3 * specular_color[1] * light_color[1];
		color[2] = scalar3 * specular_color[2] * light_color[2];
	}
	else {
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
	}
}


void diffuse_reflection(double *normal, double *object_light_vector, double *light_color, double *diffuse_color, double *color) {
	double scalar = dot_product(normal, object_light_vector);
	
	if (scalar > 0) {
		color[0] = scalar * (diffuse_color[0] * light_color[0]);
		color[1] = scalar * (diffuse_color[1] * light_color[1]);
		color[2] = scalar * (diffuse_color[2] * light_color[2]);
	}
	else {
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
	}
}


double fang(double angular_a0, double theta, double *direction, double *distance) {
	if (theta == 0 && direction[0] == 0 && direction[1] == 0 && direction[2] == 0) {
		return 1.0;
	} else {
		theta = (theta * M_PI) / 180;			// convert to radians
		double scalar = dot_product(direction, distance);
		if (scalar > cos(theta)) {
			return pow(scalar, angular_a0);
		} else {
			return 0;
		}
	}
}


double frad(double radial_a0, double radial_a1, double radial_a2, double distance) {
	if (distance < INFINITY) {
		return (1 / (radial_a0 + (radial_a1 * distance) + (radial_a2 * pow(distance, 2))));
	} else {
		return 1.0;
	}
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

void copy_vector(double *src, double *dest) {
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

double clamp(double input, double min, double max) {
	if (input > max) { return max; }
	if (input < min) { return min; }
	return input;
}

void reflect_vector(double *a, double *b, double *dest) {
	double temp[3];
	
	double scalar = dot_product(a, b);
	scale_vector(2 * scalar, b, temp);
	sub_vectors(a, temp, dest);
}