#ifndef READWAV_HH
#define READWAV_HH

#include <vector>
#include <string>
#include <utility>

std::pair<std::vector<short>, int> readUncompressedWavFile(std::string file_name);

#endif
