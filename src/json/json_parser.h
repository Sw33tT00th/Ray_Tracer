//
// Created by John Loudon on 9/22/16.
//

#ifndef RAY_TRACER_JSON_PARSER_H
#define RAY_TRACER_JSON_PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// TYPE DEFINITIONS
typedef enum {
	FALSE,			// 0
	TRUE			// 1
} boolean;

typedef enum {
	JSON_STRING,	// 0
	JSON_NUMBER,	// 1
	JSON_BOOLEAN,	// 2
	JSON_OBJECT,	// 3
	JSON_ARRAY		// 4
} json_element_type;

typedef struct _Json_Element {
	json_element_type type;
	char* key;
	union {
		char* data_string;
		double data_number;
		boolean data_bool;
		struct _Json_Element *data_element;
	} data;
	int count;
} Json_Element;

// FUNCTION SIGNATURES
int parse_json(FILE* json_file, Json_Element *root_element);
int get_data_type(int next_character);
int skip_whitespace(FILE* json_file, int *line_number);
int expect_character(int current_character, int expected_character, int line_number);
int read_string(FILE* json_file, int *line_number, char *destination);

#endif //RAY_TRACER_JSON_PARSER_H
