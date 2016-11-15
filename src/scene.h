//
// Created by John Loudon on 10/1/16.
//

#ifndef RAY_TRACER_SCENE_H
#define RAY_TRACER_SCENE_H

#include "json/json_parser.h"

typedef enum {
	OBJ_SPHERE,		// 0
	OBJ_PLANE,		// 1
	OBJ_CAMERA,		// 2
	OBJ_LIGHT		// 3
} object_type;

typedef struct {
	double width;
	double height;
} Camera;

typedef struct {
	double color[3];
	double specular_color[3];
	double radius;
	double reflectivity;
	double refractivity;
} Sphere;

typedef struct {
	double color[3];
	double normal[3];
	double reflectivity;
	double refractivity;
} Plane;

typedef struct _light {
	double color[3];
	double radialA0;
	double radialA1;
	double radialA2;
	double angularA0;
	double theta;
	double direction[3];
} Light;

typedef struct _object {
	double center[3];
	object_type type;
	union {
		Sphere sphere;
		Plane plane;
		Camera camera;
		Light light;
	} data;
	boolean has_data;
} Object;

int build_scene(Object *objects, Object *lights, Json_Element *root_element);
Object get_object(Json_Element *object_data);

#endif //RAY_TRACER_SCENE_H
