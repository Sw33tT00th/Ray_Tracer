//
// Created by John Loudon on 9/22/16.
//

#ifndef RAY_TRACER_MAIN_H
#define RAY_TRACER_MAIN_H

#include "scene.h"

double intersect_sphere(double *origin, double *direction, Object *current_object, double *color);
double intersect_plane(double *origin, double *direction, Object *current_object, double *color);
int validate_parameters(int argc, char *argv[]);

#endif //RAY_TRACER_MAIN_H

