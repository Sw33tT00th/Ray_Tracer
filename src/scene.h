//
// Created by John Loudon on 10/1/16.
//

#ifndef RAY_TRACER_SCENE_H
#define RAY_TRACER_SCENE_H

typedef enum {
	OBJ_SPHERE,		// 0
	OBJ_PLAIN,		// 1
	OBJ_CAMERA		// 2
} object_type;

typedef struct {
	double color[3];
	double radius;
} Sphere;

typedef struct {
	double color[3];
	double normal[3];
} Plain;

typedef struct _object {
	double center[3];
	object_type type;
	union {
		Sphere sphere;
		Plain plain;
	} data;
} Object;



#endif //RAY_TRACER_SCENE_H
