.PHONY: build_image

build_image:
	g++ -g src/build_image.cpp src/readWav.cpp -o build_image -std=c++14 -O3 -Wall -Wno-sign-compare -Wextra -I include
