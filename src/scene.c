//
// Created by John Loudon on 10/1/16.
//

#include <string.h>
#include "scene.h"

/**
 * BUILD SCENE
 * @param objects
 * @param root_element
 * @return
 * @description builds the array of objects in a scene from json data
 */
int build_scene(Object *objects, Json_Element *root_element) {
	Json_Element *current_element = root_element;
	if (current_element->type != JSON_ARRAY) {
		fprintf(stderr, "First element must be an array");
		return 1;
	}
	// enter the main array
	current_element = current_element->data.data_element;
	
	int i = 0;
	
	while(current_element->has_data == TRUE) {
		objects[i] = get_object(current_element);
		current_element++;
		i++;
	}
	return 0;
}

Object get_object(Json_Element *object_data) {
	Object to_return;
	// enter the object
	object_data = object_data->data.data_element;
	
	// close program if first element isn't the type
	if (strcmp(object_data->key, "type") != 0) {
		fprintf(stderr, "First element of object must be object type\n\n");
		exit(1);
	}
	
	// handle data as a string and check if it is a camera, sphere or plain
	if (strcmp(object_data->data.data_string, "camera") == 0) {
		to_return.type = OBJ_CAMERA;
		object_data++;
		// create a camera object
		to_return.center[0] = 0;
		to_return.center[1] = 0;
		to_return.center[2] = 0;
		int i;
		for (i = 0; i < 2; i++) {
			if (strcmp(object_data->key, "width") == 0) {
				to_return.data.camera.width = object_data->data.data_number;
			}
			else if (strcmp(object_data->key, "height") == 0) {
				to_return.data.camera.height = object_data->data.data_number;
			}
			else {
				fprintf(stderr, "ERROR: camera can only have width and height values\n\n");
				exit(1);
			}
			object_data++;
		}
	}
	else if (strcmp(object_data->data.data_string, "sphere") == 0) {
		to_return.type = OBJ_SPHERE;
		object_data++;
		// create a sphere
		int i;
		for (i = 0; i < 3; i++) {
			if (strcmp(object_data->key, "color") == 0) {
				// set color
				Json_Element *color_array = object_data->data.data_element;
				to_return.data.sphere.color[0] = color_array->data.data_number;
				color_array++;
				to_return.data.sphere.color[1] = color_array->data.data_number;
				color_array++;
				to_return.data.sphere.color[2] = color_array->data.data_number;
			}
			else if (strcmp(object_data->key, "position") == 0) {
				// set position
				Json_Element *position_array = object_data->data.data_element;
				to_return.center[0] = position_array->data.data_number;
				position_array++;
				to_return.center[1] = position_array->data.data_number;
				position_array++;
				to_return.center[2] = position_array->data.data_number;
			}
			else if (strcmp(object_data->key, "radius") == 0) {
				// set radius
				to_return.data.sphere.radius = object_data->data.data_number;
			}
			else {
				fprintf(stderr, "ERROR: sphere can only have color, position, and radius\n\n");
				exit(1);
			}
			object_data++;
		}
		
	}
	else if (strcmp(object_data->data.data_string, "plane") == 0) {
		to_return.type = OBJ_PLANE;
		object_data++;
		// create a plain
		int i;
		for (i = 0; i < 3; i++) {
			if (strcmp(object_data->key, "color") == 0) {
				// set color
				Json_Element *color_array = object_data->data.data_element;
				to_return.data.plane.color[0] = color_array->data.data_number;
				color_array++;
				to_return.data.plane.color[1] = color_array->data.data_number;
				color_array++;
				to_return.data.plane.color[2] = color_array->data.data_number;
			}
			else if (strcmp(object_data->key, "position") == 0) {
				// set position
				Json_Element *position_array = object_data->data.data_element;
				to_return.center[0] = position_array->data.data_number;
				position_array++;
				to_return.center[1] = position_array->data.data_number;
				position_array++;
				to_return.center[2] = position_array->data.data_number;
			}
			else if (strcmp(object_data->key, "normal") == 0) {
				// set normal
				Json_Element *normal_vector = object_data->data.data_element;
				to_return.data.plane.normal[0] = normal_vector->data.data_number;
				normal_vector++;
				to_return.data.plane.normal[1] = normal_vector->data.data_number;
				normal_vector++;
				to_return.data.plane.normal[2] = normal_vector->data.data_number;
			}
			else {
				fprintf(stderr, "ERROR: plane can only have color, position, and normal\n\n");
				exit(1);
			}
			object_data++;
		}
	}
	else {
		fprintf(stderr, "ERROR: %s is not a recognized object type\n\n", object_data->data.data_string);
		exit(1);
	}
	
	return to_return;
}