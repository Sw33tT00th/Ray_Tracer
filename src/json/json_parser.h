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
	JSON_STRING,	// 1
	JSON_NUMBER,	// 2
	JSON_BOOLEAN,	// 3
	JSON_OBJECT,	// 4
	JSON_ARRAY		// 5
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
	boolean has_data;
} Json_Element;

// FUNCTION SIGNATURES
int parse_json(FILE* json_file, Json_Element *root_element);
int parse_array(Json_Element *current_element, FILE* json_file, int *line_number);
int parse_object(Json_Element *current_element, FILE* json_file, int *line_number);
int get_data_type(int next_character);
int skip_whitespace(FILE* json_file, int *line_number);
int expect_character(int current_character, int expected_character, int line_number);
char* read_string(FILE* json_file, int *line_number);
int get_next_element(Json_Element *current_element, FILE* json_file, int *line_number, int current_character);
void print_current_element(Json_Element *current_element);

#endif //RAY_TRACER_JSON_PARSER_H
