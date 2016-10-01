//
// Created by John Loudon on 10/1/16.
//

#ifndef RAY_TRACER_SCENE_H
#define RAY_TRACER_SCENE_H

#include "json/json_parser.h"

typedef enum {
	OBJ_SPHERE,		// 0
	OBJ_PLANE,		// 1
	OBJ_CAMERA		// 2
} object_type;

typedef struct {
	double width;
	double height;
} Camera;

typedef struct {
	double color[3];
	double radius;
} Sphere;

typedef struct {
	double color[3];
	double normal[3];
} Plane;

typedef struct _object {
	double center[3];
	object_type type;
	union {
		Sphere sphere;
		Plane plane;
		Camera camera;
	} data;
} Object;

int build_scene(Object *objects, Json_Element *root_element);
Object get_object(Json_Element *object_data);

#endif //RAY_TRACER_SCENE_H