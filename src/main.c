#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "ppm/helpers.h"
#include "json/json_parser.h"

int main(int argc, char *argv[]) {
	Json_Element *root_element;
	FILE* json_file = fopen("/Users/Sw33tT00th/Documents/Fall 2016/CS 430/Ray_Tracer/test_data/test_data.json", "r");
	
	if (json_file == NULL) {
		fprintf(stderr, "Failed to open json file\n\nClosing Program");
		exit(1);
	}
	
	int error_check = 0;
	error_check = parse_json(json_file, root_element);
	if (error_check) {
		printf("Closing Program\n");
		fclose(json_file);
	}
	
	printf("Program completed\n");
	return 0;
}