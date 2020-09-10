#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include "readWav.hh"

using namespace std;
namespace {
	int to_int(char* data, int data_size){
		int ret=0;
		for(int i=0; i<data_size; i++)
		    ret+=(int((unsigned char)data[i]))*(1<<(8*i));
		return ret;
	}


	static void invalidFileFormatErrorMSG(string msg){
		cerr<<"Problem with file format, please make sure that the file is a correct and uncompressed .wav file!"<<" ; "<<msg<<endl;
		abort();
	}
}

// Returns pair (samples, sample rate)
pair<vector<short>, int> readUncompressedWavFile(string file_name){
    ifstream fin(file_name.c_str());

    char ChunkID[5]= {};
    fin.read(ChunkID, 4);
    if(string(ChunkID)!="RIFF") invalidFileFormatErrorMSG("RIFF "+file_name);

    char ChunkSize[5] = {};
    fin.read(ChunkSize, 4);

    char Format[5] = {};
    fin.read(Format, 4);
    if(string(Format)!="WAVE") invalidFileFormatErrorMSG("WAVE");

    char subChunk1ID[5] = {};
    fin.read(subChunk1ID,4);
    if(string(subChunk1ID)!="fmt ") invalidFileFormatErrorMSG("fmt ");

    char subChunk1Size[5] = {};
    fin.read(subChunk1Size,4);
    if(to_int(subChunk1Size,4)!=16) invalidFileFormatErrorMSG("subchunk");

    char audioFormat[3] = {};
    fin.read(audioFormat,2);
    if(to_int(audioFormat,2)!=1) invalidFileFormatErrorMSG("audioformat");

    char numOfChannels[3] = {};
    fin.read(numOfChannels,2);
    int numberOfChannels = to_int(numOfChannels,2);

    char sampleRate[5] = {};
    fin.read(sampleRate,4);
    int sample_rate=to_int(sampleRate,4);

    char byteRate[5] = {};
    fin.read(byteRate,4);

    char blockAlign[3] = {};
    fin.read(blockAlign,2);

    char bitsPerSample[3] = {};
    fin.read(bitsPerSample,2);
    int bits_per_sample=to_int(bitsPerSample,2);
    assert(bits_per_sample==16);

    if(to_int(byteRate,4)!=to_int(sampleRate,4)*numberOfChannels*to_int(bitsPerSample,2)/8)
        invalidFileFormatErrorMSG("byterate");

    char subChunk2ID[5] = {};
    char subChunk2Size[5] = {};
    while(true){        
        fin.read(subChunk2ID,4);
        fin.read(subChunk2Size,4);
        if(string(subChunk2ID) == "data") break;
        else{
            // Skip this chunk
            int buf_size = to_int(subChunk2Size, 4);
            vector<char> buf(buf_size);
            fin.read(&(buf[0]), buf_size);
        }
    }

    
    int data_array_size=(8*to_int(subChunk2Size,4))/(to_int(bitsPerSample,2)*numberOfChannels);
    int sample_size_in_bytes=to_int(bitsPerSample,2)/8;

    assert(sample_size_in_bytes == 2);
    vector<short> buf(data_array_size*numberOfChannels);
    fin.read((char*)&buf[0], buf.size()*sample_size_in_bytes);

    if(numberOfChannels == 2){
        // Add channels together
        for(int i = 0; i < buf.size()/numberOfChannels; i++){
            int sum = 0;
            for(int j = 0; j < numberOfChannels; j++)
                sum += buf[numberOfChannels*i + j];
            buf[i] = sum / numberOfChannels;
        }
        buf.resize(buf.size()/2);
    }

    return {buf, sample_rate};
}
