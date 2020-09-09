#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include "readWav.hh"

// Code kindly provided by OOliOO @ IRCNet

using namespace std;
namespace {
	int to_int(char* data, int data_size){
		int ret=0;
		for(int i=0; i<data_size; i++)
		    ret+=(int((unsigned char)data[i]))*(1<<(8*i));
		return ret;
	}


	static vector<short> invalidFileFormatErrorMSG(string msg){
		cerr<<"Problem with file format, please make sure that the file is a correct and uncompressed .wav file!"<<" ; "<<msg<<endl;
		abort();
		return vector<short>();
	}
}


vector<short> readUncompressedWavFile(string file_name){
    ifstream fin(file_name.c_str());

    char ChunkID[5]= {};
    fin.read(ChunkID, 4);
    if(string(ChunkID)!="RIFF") return invalidFileFormatErrorMSG("RIFF "+file_name);

    char ChunkSize[5] = {};
    fin.read(ChunkSize, 4);

    char Format[5] = {};
    fin.read(Format, 4);
    if(string(Format)!="WAVE") return invalidFileFormatErrorMSG("WAVE");

    char subChunk1ID[5] = {};
    fin.read(subChunk1ID,4);
    if(string(subChunk1ID)!="fmt ") return invalidFileFormatErrorMSG("fmt ");

    char subChunk1Size[5] = {};
    fin.read(subChunk1Size,4);
    if(to_int(subChunk1Size,4)!=16) return invalidFileFormatErrorMSG("subchunk");

    char audioFormat[3] = {};
    fin.read(audioFormat,2);
    if(to_int(audioFormat,2)!=1) return invalidFileFormatErrorMSG("audioformat");

    char numOfChannels[3] = {};
    fin.read(numOfChannels,2);
    int numberOfChannels = to_int(numOfChannels,2);

    char sampleRate[5] = {};
    fin.read(sampleRate,4);
    int sample_rate=to_int(sampleRate,4);
    assert(sample_rate==44100);

    char byteRate[5] = {};
    fin.read(byteRate,4);

    char blockAlign[3] = {};
    fin.read(blockAlign,2);

    char bitsPerSample[3] = {};
    fin.read(bitsPerSample,2);
    int bits_per_sample=to_int(bitsPerSample,2);
    assert(bits_per_sample==16);

    if(to_int(byteRate,4)!=to_int(sampleRate,4)*numberOfChannels*to_int(bitsPerSample,2)/8)
        return invalidFileFormatErrorMSG("byterate");

    char subChunk2ID[5] = {};
    char subChunk2Size[5] = {};
    while(true){        
        fin.read(subChunk2ID,4);
        fin.read(subChunk2Size,4);
        if(string(subChunk2ID) == "data") break;
        else{
            // Skip this chunk
            int buf_size = to_int(subChunk2Size, 4);
            cerr << "Skipping chunk " << string(subChunk2ID) << " (size = " << buf_size << " bytes)" << endl;
            vector<char> buf(buf_size);
            fin.read(&(buf[0]), buf_size);
        }
    }

    
    int data_array_size=(8*to_int(subChunk2Size,4))/(to_int(bitsPerSample,2)*numberOfChannels);
    int sample_size_in_bytes=to_int(bitsPerSample,2)/8;

    assert(sample_size_in_bytes == 2);
    assert(numberOfChannels <= 2);
    vector<short> buf(data_array_size*numberOfChannels);
    fin.read((char*)&buf[0], buf.size()*sample_size_in_bytes);

    if(numberOfChannels == 2){
        // Add left and right channels together
        for(int i = 0; i < buf.size()/2; i++){
            buf[i] = ((int)buf[2*i] + (int)buf[2*i+1]) / 2;
        }
        buf.resize(buf.size()/2);
    }

    return buf;
}
