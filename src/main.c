#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "ppm/helpers.h"
#include "json/json_parser.h"

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
	
	/*Json_Element *current_element;
	
	current_element = root_element;
	printf("Root Element Type: %d\n", current_element->type);
	current_element = current_element->data.data_element;
	printf("First element of array type: %d\n", current_element->type);
	current_element = current_element->data.data_element;
	printf("First element of object type: %d\n", current_element->type);
	printf("\tKEY: %s\n\tVALUE: %f\n", current_element->key, current_element->data.data_number);
	current_element = current_element + 1;
	printf("Second element of object type: %d\n", current_element->type);
	printf("\tKEY: %s\n\tVALUE: %s\n", current_element->key, current_element->data.data_string);
	current_element = current_element + 1;
	printf("Second element of object type: %d\n", current_element->type);*/
	
	fprintf(stderr, "\nProgram completed\n");
	return 0;
}