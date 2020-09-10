.PHONY: spectrogram

spectrogram:
	$(CXX) -g src/spectrogram.cpp src/readWav.cpp -o bin/spectrogram -std=c++14 -O3 -Wall -Wno-sign-compare -Wextra -I include
