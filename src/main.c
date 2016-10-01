#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "ppm/helpers.h"
#include "json/json_parser.h"
#include "scene.h"

int main(int argc, char *argv[]) {
	Json_Element *root_element;
	FILE* json_file = fopen(argv[3], "r");
	
	if (json_file == NULL) {
		fprintf(stderr, "Failed to open json file\n\nClosing Program\n");
		exit(1);
	}
	
	// allocate 128 blocks of json element sized memory
	root_element = malloc(sizeof(Json_Element) * 1);
	
	int error_check = 0;
	error_check = parse_json(json_file, root_element);
	if (error_check) {
		fprintf(stderr, "Closing Program\n");
		fclose(json_file);
	}
	
	Object *objects = malloc(sizeof(Object) * 128);
	
	build_scene(objects, root_element);
	
	
	
	fprintf(stderr, "\nProgram completed\n");
	return 0;
}