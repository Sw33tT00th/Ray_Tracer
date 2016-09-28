#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "json/json_parser.h"

int main(int argc, char *argv[]) {
	Json_Element *root_element;
	FILE* json_file = fopen("test_data/test_data.json", "r");
	int error_check = 0;
	
	error_check = parse_json(json_file, root_element);
	if (error_check) {
		printf("Closing Program\n");
		fclose(json_file);
	}
	
	return 0;
}