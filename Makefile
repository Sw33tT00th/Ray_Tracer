all:
	gcc -O3 -o bin/main src/ppm/helpers.c src/ppm/header.c src/ppm/p6.c src/json/json_parser.c src/scene.c src/main.c

clean:
	rm bin/main