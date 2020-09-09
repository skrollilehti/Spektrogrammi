#ifndef READWAV_HH
#define READWAV_HH

#include <vector>
#include <string>

std::vector<short> readUncompressedWavFile(std::string file_name);

#endif
