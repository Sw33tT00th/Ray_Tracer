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

#endif //RAY_TRACER_MAIN_H

