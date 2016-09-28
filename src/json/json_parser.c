//
// Created by John Loudon on 9/22/16.
//

#include <string.h>
#include <ctype.h>
#include "json_parser.h"
#include "../ppm/helpers.h"

/******************************************************************************
 * PARSE JSON
 * @param FILE* json_file
 * @param Json_Element* root_element (definition in json_parser.h)
 * @return	0	-	file was parsed correctly
 * 			1	-	json file was empty
 * 			2	-	file didn't start with an array bracket '['
 * 			3	-	initial array was empty
 * @description read json file into a structure
 *****************************************************************************/
int parse_json(FILE* json_file, Json_Element *root_element) {
	int line_number = 1;
	int current_character;
	
	// skip whitespace, and if it finds the end of the file return 1
	if (skip_whitespace(json_file, &line_number)) {
		fprintf(stderr, "JSON file was empty\n\n");
		return 1;
	}
	
	// get the next character without removing if from the stream
	current_character = peek_next_char(json_file);
	
	// Check to see if the first non-whitespace character is the start of an array.
	// File must start with an array.
	if (expect_character(current_character, '[', line_number)) {
		return 2;
	}
	
	// remove the [
	getc(json_file);
	
	// get the next character
	current_character = peek_next_char(json_file);
	
	if (current_character == ']') {
		fprintf(stderr, "JSON array was empty, no scene data found\n\n");
		return 3;
	}
	
	
	return 0;
}

/******************************************************************************
 * GET DATA TYPE
 * @param next_character
 * @return integer representing a type (0-4) or -1 if something goes wrong
 * @description reads the first character and returns a number representing the
 * type based on the enumeration defined in json_parser.h
 *****************************************************************************/
int get_data_type(int next_character) {
	// check if the character is a " indicating that the data is a string
	if (next_character == '"') { return JSON_STRING; }
	
	// check if the next character is a number
	if (isdigit(next_character)) { return JSON_NUMBER; }
	
	// check if the next character is t or f indicating a boolean
	if (next_character == 't' || next_character == 'f')
	{ return JSON_BOOLEAN; }
	
	// check if the next character is a { indicating a nested json object
	if (next_character == '{') { return JSON_OBJECT; }
	
	// check if the next character is a [ indicating an array
	if (next_character == '[') { return JSON_ARRAY; }
	
	// return -1 indicating that the next character was something unexpected
	return -1;
}

/******************************************************************************
 * SKIP WHITESPACE
 * @param json_file
 * @param line_number
 * @return	0	-	the file pointer was moved past all whitespace
 * 			1	-	there was only whitespace before the end of the file
 * @description move the file pointer past any whitespace characters
 *****************************************************************************/
int skip_whitespace(FILE* json_file, int *line_number) {
	// get the first character without removing it from the stream
	int current_character = peek_next_char(json_file);
	
	// while the character is whitespace remove the next character and check
	while (isspace(current_character)) {
		// increment line counter
		if (current_character == '\n') { *line_number += 1; }
		
		// remove the next character from the stream
		getc(json_file);
		
		// get the next character
		current_character = peek_next_char(json_file);
		
		// check for end of file
		if (current_character == EOF) {
			return 1;
		}
	}
	
	return 0;
}

/******************************************************************************
 * EXPECT CHARACTER
 * @param current_character
 * @param expected_character
 * @param line_number
 * @return 	0	-	character is what was expected
 * 			1	-	character was unexpected
 * @description check to see if a character is what you expect
 *****************************************************************************/
int expect_character(int current_character, int expected_character, int line_number) {
	// check if the two characters are equal
	if (current_character != expected_character) {
		// output an error message
		fprintf(stderr, "Unexpected character on line %d:\n\texpected: \"%c\"\n\tfound: \"%c\"\n",
				line_number, expected_character, current_character);
		return 1;
	}
	return 0;
}

