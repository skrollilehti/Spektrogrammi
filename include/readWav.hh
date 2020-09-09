#ifndef READWAV_HH
#define READWAV_HH

#include <vector>
#include <string>

// Author : Jarno Alanko
// Data: 8.5. 2015

std::vector<short> readUncompressedWavFile(std::string file_name);

#endif
