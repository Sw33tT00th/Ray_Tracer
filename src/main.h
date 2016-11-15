//
// Created by John Loudon on 9/22/16.
//

#ifndef RAY_TRACER_MAIN_H
#define RAY_TRACER_MAIN_H

#include "scene.h"

double intersect_sphere(double *origin, double *direction, Object *current_object);
double intersect_plane(double *origin, double *direction, Object *current_object);
int validate_parameters(int argc, char *argv[]);
void scale_vector(double scale, double *vector, double *dest);
void multiply_vectors(double *a, double *b, double *dest);
void add_vectors(double *a, double *b, double *dest);
void sub_vectors(double *a, double *b, double *dest);
double clamp(double input, double min, double max);
void specular_highlight(double *normal, double *object_light_vector, double *reflected_vector, double *direction, double *specular_color, double *light_color, double *color);
double fang(double angular_a0, double theta, double *direction, double *distance);
double frad(double radial_a0, double radial_a1, double radial_a2, double distance);
void copy_vector(double *src, double *dest);
void reflect_vector(double *a, double *b, double *dest);
void diffuse_reflection(double *normal, double *object_light_vector, double *light_color, double *diffuse_color, double *color);
void shoot(Object *objects, double *origin, double *direction, Object *lights, double *color_to_write, int reflection_index);

#endif //RAY_TRACER_MAIN_H

