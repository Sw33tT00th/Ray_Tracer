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
 * 			4	-	file ended unexpectedly
 * 			5	-	unexpected character
 * 			6	-	generic error
 * @description read json file into a structure
 *****************************************************************************/
int parse_json(FILE* json_file, Json_Element *root_element) {
	int line_number = 1;
	int current_character;
	char *temp;
	
	// skip whitespace, and if it finds the end of the file return 1
	if (skip_whitespace(json_file, &line_number)) {
		fprintf(stderr, "ERROR: JSON file was empty\n\n");
		return 1;
	}
	
	// get the next character
	current_character = getc(json_file);
	
	Json_Element *current_element = root_element;
	
	while (current_character != EOF) {
		switch (current_character) {
			case '[':
				// HANDLE ARRAY
				
				// set the current element type to array
				current_element->type = JSON_ARRAY;
				
				// allocate memory for the array
				current_element->data.data_element = malloc(sizeof(Json_Element) * 128);
				
				// set the current element to the array itself
				current_element = current_element->data.data_element;
				
				// parse the array
				if (parse_array(current_element, json_file, &line_number)) {
					return 6;
				}
				break;
			
			case '{':
				// HANDLE OBJECT
				
				// set the current element type to object
				current_element->type = JSON_OBJECT;
				
				// allocate memory for the object
				current_element->data.data_element = malloc(sizeof(Json_Element) * 8);
				
				// set the current element to the object itself
				current_element = current_element->data.data_element;
				
				// parse the object
				if (parse_object(current_element, json_file, &line_number)) {
					return 6;
				}
				break;
			
			case '"':
				// PARSE KEY AND HANDLE GENERIC ELEMENT
				
				// read and store the key
				current_element->key = read_string(json_file, &line_number);
				if (strcmp(current_element->key, "\0") == 0) {
					return 6;
				}
				
				// remove whitespace
				if (skip_whitespace(json_file, &line_number)) {
					fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
					return 1;
				}
				
				// check for a :
				current_character = getc(json_file);
				if (expect_character(current_character, ':', line_number)) {
					return 5;
				}
				
				// remove whitespace
				if (skip_whitespace(json_file, &line_number)) {
					fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
					return 1;
				}
				
				current_character = peek_next_char(json_file);
				current_element->type = get_data_type(current_character);
				switch (current_element->type) {
					case JSON_ARRAY:
						// HANDLE ARRAY
						
						// set the current element type to array
						current_element->type = JSON_ARRAY;
						
						// allocate memory for the array
						current_element->data.data_element = malloc(sizeof(Json_Element) * 128);
						
						// set the current element to the array itself
						current_element = current_element->data.data_element;
						
						// parse the array
						if (parse_array(current_element, json_file, &line_number)) {
							return 6;
						}
						break;
					
					case JSON_OBJECT:
						// HANDLE OBJECT
						
						// set the current element type to object
						current_element->type = JSON_OBJECT;
						
						// allocate memory for the object
						current_element->data.data_element = malloc(sizeof(Json_Element) * 8);
						
						// set the current element to the object itself
						current_element = current_element->data.data_element;
						
						// parse the object
						if (parse_object(current_element, json_file, &line_number)) {
							return 6;
						}
						break;
					
					case JSON_NUMBER:
						// HANDLE NUMBER
						fscanf(json_file, "%lf", &current_element->data.data_number);
						break;
					
					case JSON_STRING:
						// HANDLE STRING
						
						current_element->data.data_string = read_string(json_file, &line_number);
						break;
					
					case JSON_BOOLEAN:
						// HANDLE BOOLEAN
						
						temp = read_string(json_file, &line_number);
						if (strcmp(temp, "true") == 0) {
							current_element->data.data_bool = TRUE;
						}
						if (strcmp(temp, "false") == 0) {
							current_element->data.data_bool = FALSE;
						}
						break;
					
					default:
						fprintf(stderr, "ERROR: How did you get here?\n\n");
						break;
				}
				// increment current element
				current_element += 1;
				break;
				
			case ',':
				// remove the comma
				getc(json_file);
				break;
			
			default:
				fprintf(stderr, "from parse_json\n");
				fprintf(stderr,
						"ERROR: Unexpected character at line %d\n\tExpected: '\"', '[', or '{'\n\tFound: %c\n\n",
						line_number, current_character);
				return 5;
		}
		// remove whitespace
		// while the character is whitespace remove the next character and check
		while (isspace(current_character)) {
			// increment line counter
			if (current_character == '\n') { line_number += 1; }
			
			// remove the next character from the stream
			getc(json_file);
			
			// get the next character
			current_character = peek_next_char(json_file);
			
		}
		getc(json_file);
		current_character = getc(json_file);
	}
	
	return 0;
}

/**
 * PARSE ARRAY
 * @param current_element
 * @param json_file
 * @param line_number
 * @return
 * @description parse an array
 */
int parse_array(Json_Element *current_element, FILE* json_file, int *line_number) {
	char *temp;
	// remove whitespace
	if (skip_whitespace(json_file, line_number)) {
		fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
		return 1;
	}
	int current_character = getc(json_file);
	while (current_character != ']') {
		switch (current_character) {
			case '[':
				// HANDLE ARRAY
				
				// set the current element type to array
				current_element->type = JSON_ARRAY;
				
				// allocate memory for the array
				current_element->data.data_element = malloc(sizeof(Json_Element) * 128);
				
				// set the current element to the array itself
				current_element = current_element->data.data_element;
				
				// parse the array
				if (parse_array(current_element, json_file, line_number)) {
					return 6;
				}
				break;
			
			case '{':
				// HANDLE OBJECT
				
				// set the current element type to object
				current_element->type = JSON_OBJECT;
				
				// allocate memory for the object
				current_element->data.data_element = malloc(sizeof(Json_Element) * 8);
				
				// set the current element to the object itself
				current_element = current_element->data.data_element;
				
				// parse the object
				if (parse_object(current_element, json_file, line_number)) {
					return 6;
				}
				break;
			
			case '"':
				// PARSE KEY AND HANDLE GENERIC ELEMENT
				
				// read and store the key
				current_element->key = read_string(json_file, line_number);
				if (strcmp(current_element->key, "\0") == 0) {
					return 6;
				}
				
				// remove whitespace
				if (skip_whitespace(json_file, line_number)) {
					fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
					return 1;
				}
				
				// check for a :
				current_character = getc(json_file);
				if (expect_character(current_character, ':', *line_number)) {
					return 5;
				}
				
				// remove whitespace
				if (skip_whitespace(json_file, line_number)) {
					fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
					return 1;
				}
				
				current_character = peek_next_char(json_file);
				current_element->type = get_data_type(current_character);
				switch (current_element->type) {
					case JSON_ARRAY:
						// HANDLE ARRAY
						
						// set the current element type to array
						current_element->type = JSON_ARRAY;
						
						// allocate memory for the array
						current_element->data.data_element = malloc(sizeof(Json_Element) * 128);
						
						// set the current element to the array itself
						current_element = current_element->data.data_element;
						
						// parse the array
						if (parse_array(current_element, json_file, line_number)) {
							return 6;
						}
						break;
					
					case JSON_OBJECT:
						// HANDLE OBJECT
						
						// set the current element type to object
						current_element->type = JSON_OBJECT;
						
						// allocate memory for the object
						current_element->data.data_element = malloc(sizeof(Json_Element) * 8);
						
						// set the current element to the object itself
						current_element = current_element->data.data_element;
						
						// parse the object
						if (parse_object(current_element, json_file, line_number)) {
							return 6;
						}
						break;
					
					case JSON_NUMBER:
						// HANDLE NUMBER
						fscanf(json_file, "%lf", &current_element->data.data_number);
						break;
					
					case JSON_STRING:
						// HANDLE STRING
						
						current_element->data.data_string = read_string(json_file, line_number);
						break;
					
					case JSON_BOOLEAN:
						// HANDLE BOOLEAN
						
						temp = read_string(json_file, line_number);
						if (strcmp(temp, "true") == 0) {
							current_element->data.data_bool = TRUE;
						}
						if (strcmp(temp, "false") == 0) {
							current_element->data.data_bool = FALSE;
						}
						break;
					
					default:
						fprintf(stderr, "ERROR: How did you get here?\n\n");
						return 4;
				}
				// increment current element
				current_element += 1;
				break;
			
			case ',':
				// remove the comma
				getc(json_file);
				break;
			
			default:
				fprintf(stderr, "from parse_array\n");
				fprintf(stderr,
						"ERROR: Unexpected character at line %d\n\tExpected: '\"', '[', or '{'\n\tFound: %c\n\n",
						*line_number, current_character);
				return 2;
		}
		// remove whitespace
		if (skip_whitespace(json_file, line_number)) {
			fprintf(stderr, "from parse_array\n");
			fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
			return 1;
		}
		current_character = peek_next_char(json_file);
	}
	getc(json_file);
	return 0;
}

/**
 * PARSE OBJECT
 * @param current_element
 * @param json_file
 * @param line_number
 * @return
 * @description parse an object
 */
int parse_object(Json_Element *current_element, FILE* json_file, int *line_number) {
	char* temp;
	// remove whitespace
	if (skip_whitespace(json_file, line_number)) {
		fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
		return 1;
	}
	
	int current_character = getc(json_file);
	while (current_character != '}') {
		switch (current_character) {
			case '[':
				// HANDLE ARRAY
				
				// set the current element type to array
				current_element->type = JSON_ARRAY;
				
				// allocate memory for the array
				current_element->data.data_element = malloc(sizeof(Json_Element) * 128);
				
				// set the current element to the array itself
				current_element = current_element->data.data_element;
				
				// parse the array
				if (parse_array(current_element, json_file, line_number)) {
					return 6;
				}
				break;
			
			case '{':
				// HANDLE OBJECT
				
				// set the current element type to object
				current_element->type = JSON_OBJECT;
				
				// allocate memory for the object
				current_element->data.data_element = malloc(sizeof(Json_Element) * 8);
				
				// set the current element to the object itself
				current_element = current_element->data.data_element;
				
				// parse the object
				if (parse_object(current_element, json_file, line_number)) {
					return 6;
				}
				break;
			
			case '"':
				// PARSE KEY AND HANDLE GENERIC ELEMENT
				
				// read and store the key
				current_element->key = read_string(json_file, line_number);
				if (strcmp(current_element->key, "\0") == 0) {
					return 6;
				}
				//printf("found key: %s\n", current_element->key);
				// remove whitespace
				if (skip_whitespace(json_file, line_number)) {
					fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
					return 1;
				}
				
				// check for a :
				current_character = peek_next_char(json_file);
				if (expect_character(current_character, ':', *line_number)) {
					return 5;
				}
				
				getc(json_file);
				
				// remove whitespace
				if (skip_whitespace(json_file, line_number)) {
					fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
					return 1;
				}
				
				current_character = peek_next_char(json_file);
				current_element->type = get_data_type(current_character);
				switch (current_element->type) {
					case JSON_ARRAY:
						// HANDLE ARRAY
						
						// set the current element type to array
						current_element->type = JSON_ARRAY;
						
						// allocate memory for the array
						current_element->data.data_element = malloc(sizeof(Json_Element) * 128);
						
						// set the current element to the array itself
						current_element = current_element->data.data_element;
						
						// parse the array
						if (parse_array(current_element, json_file, line_number)) {
							return 6;
						}
						break;
					
					case JSON_OBJECT:
						// HANDLE OBJECT
						
						// set the current element type to object
						current_element->type = JSON_OBJECT;
						
						// allocate memory for the object
						current_element->data.data_element = malloc(sizeof(Json_Element) * 128);
						
						// set the current element to the object itself
						current_element = current_element->data.data_element;
						
						// parse the object
						if (parse_object(current_element, json_file, line_number)) {
							return 6;
						}
						break;
					
					case JSON_NUMBER:
						
						// HANDLE NUMBER
						fscanf(json_file, "%lf", &current_element->data.data_number);
						//printf("\tKey: %s\n\tValue: %f\n", current_element->key, current_element->data.data_number);
						break;
					
					case JSON_STRING:
						// HANDLE STRING
						
						current_element->data.data_string = read_string(json_file, line_number);
						//printf("\tKey: %s\n\tValue: %s\n", current_element->key, current_element->data.data_string);
						break;
					
					case JSON_BOOLEAN:
						// HANDLE BOOLEAN
						
						temp = malloc(sizeof(char) * 5);
						int i = 0;
						while (!isspace(peek_next_char(json_file))) {
							
							// check for end of file
							if (peek_next_char(json_file) == EOF) {
								fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
								return 5;
							}
							
							// append the next character
							temp[i] = (char)getc(json_file);
							i++;
						}
						if (strcmp(temp, "true") == 0) {
							current_element->data.data_bool = TRUE;
							//printf("\tkey: %s\n\tValue: true\n", current_element->key);
						}
						else if (strcmp(temp, "false") == 0) {
							current_element->data.data_bool = FALSE;
							//printf("\tkey: %s\n\tValue: false\n", current_element->key);
						}
						else {
							fprintf(stderr, "ERROR: invalid character\n\n");
							return 5;
						}
						
						break;
					
					default:
						fprintf(stderr, "ERROR: How did you get here?\n\n");
						return 4;
				}
				// increment current element
				current_element += 1;
				break;
			
			case ',':
				// remove the comma
				getc(json_file);
				break;
			
			default:
				fprintf(stderr, "from parse_object\n");
				fprintf(stderr,
						"ERROR: Unexpected character at line %d\n\tExpected: '\"', '[', or '{'\n\tFound: %c\n\n",
						*line_number, current_character);
				return 2;
		}
		// remove whitespace
		if (skip_whitespace(json_file, line_number)) {
			fprintf(stderr, "from parse_object\n");
			fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
			return 1;
		}
		current_character = peek_next_char(json_file);
	}
	getc(json_file);
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
		fprintf(stderr, "ERROR: Unexpected character on line %d:\n\texpected: \"%c\"\n\tfound: \"%c\"\n",
				line_number, expected_character, current_character);
		return 1;
	}
	return 0;
}

/******************************************************************************
 * GET KEY
 * @param json_file
 * @param line_number
 * @param key
 * @return	0	-	everything went well and a key is now stored
 * 			1	-	something went wrong with finding a key
 * @description sets a key string for a json object
 *****************************************************************************/
char* read_string(FILE* json_file, int *line_number) {
	int max = 128;
	char *destination = malloc(sizeof(char) * 128);
	// peek the next character
	int current_character = peek_next_char(json_file);
	if (current_character == '"') {
		getc(json_file);
		current_character = peek_next_char(json_file);
	}
	
	// check for end of file
	if (current_character == EOF) {
		fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
		return "\0";
	}
	
	int i = 0;
	
	// append the characters into the string
	while (peek_next_char(json_file) != '"') {
	
		// make sure there's enough space to store the next character
		if (i == max) {
			fprintf(stderr, "ERROR: string too long, max 128 characters\n\n");
			return "\0";
		}
		
		// check for end of file
		if (peek_next_char(json_file) == EOF) {
			fprintf(stderr, "ERROR: File ended unexpectedly\n\n");
			return "\0";
		}
		
		// append the next character
		destination[i] = (char)getc(json_file);
		i++;
	}
	
	// remove the ending "
	getc(json_file);
	return destination;
}

