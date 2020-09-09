/*
 * Copyright (c) 2012 Tom Alexander
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#pragma once
#include <vector>
#include <cstring>
#include <cinttypes>
#include <iostream>

namespace
{
    std::vector<uint8_t> get_4_bytes(const uint32_t & data)
    {
        std::vector<uint8_t> ret;
        uint8_t* tmp = (uint8_t*)&data;
        ret.push_back(tmp[0]);
        ret.push_back(tmp[1]);
        ret.push_back(tmp[2]);
        ret.push_back(tmp[3]);
        return ret;
    }
    void insert_4_bytes(std::vector<uint8_t> & dest, const uint32_t & data)
    {
        std::vector<uint8_t> separated_data = get_4_bytes(data);
        std::copy(separated_data.begin(), separated_data.end(), back_inserter(dest));
    }
    std::vector<uint8_t> get_2_bytes(const uint16_t & data)
    {
        std::vector<uint8_t> ret;
        uint8_t* tmp = (uint8_t*)&data;
        ret.push_back(tmp[0]);
        ret.push_back(tmp[1]);
        return ret;
    }
    void insert_2_bytes(std::vector<uint8_t> & dest, const uint16_t & data)
    {
        std::vector<uint8_t> separated_data = get_2_bytes(data);
        copy(separated_data.begin(), separated_data.end(), back_inserter(dest));
    }

    // Added by Jarno N. Alanko
    int parse_int(uint8_t* data){
        return ((unsigned int)(data[0]) << 0) +
               ((unsigned int)(data[1]) << 8) +
               ((unsigned int)(data[2]) << 16) +
               ((unsigned int)(data[3]) << 24);
    }
}

/** 
 * Encode an array of RGB values into an array of bytes that can be written as a bitmap. The input array of RGB values starts at the top left corner. There can be no additional padding in the byte array
 * 
 * @param rgb The array of RGB values
 * @param width The width of the image in pixels
 * @param height The height of the image in pixels
 * @param output The pointer where the output will be stored
 * 
 * @return The number of bytes written to output
 */
size_t bitmap_encode_rgb(const uint8_t* rgb, int width, int height, uint8_t** output)
{
    std::vector<uint8_t> data;
    data.push_back(0x42); //B
    data.push_back(0x4D); //M
    size_t file_size_offset = data.size();
    insert_4_bytes(data, 0xFFFFFFFF); //File Size, fill later
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    size_t pixel_info_offset_offset = data.size();
    insert_4_bytes(data, 0); //pixel info offset, fill later
    insert_4_bytes(data, 40); //Size of BITMAPINFOHEADER
    insert_4_bytes(data, width);
    insert_4_bytes(data, height);
    insert_2_bytes(data, 1); //Number of color planes
    insert_2_bytes(data, 24); //Bits per pixel
    insert_4_bytes(data, 0); //No compression
    size_t raw_pixel_array_size_offset = data.size();
    insert_4_bytes(data, 0); //size of raw data in pixel array, fill later
    insert_4_bytes(data, 2835); //Horizontal Resolution
    insert_4_bytes(data, 2835); //Vertical Resolution
    insert_4_bytes(data, 0); //Number of colors
    insert_4_bytes(data, 0); //Important colors
    {
        uint32_t data_size = data.size();
        memcpy(&data[pixel_info_offset_offset], &data_size, 4);
    }
    uint32_t size_of_header = data.size();
    for (uint_fast32_t y = 0; y < height; ++y)
    {
        for (uint_fast32_t x = 0; x < width; ++x)
        {
            // BMP uses blue-green-red order
            data.push_back(rgb[y*(width*3) + x*3 + 2]);
            data.push_back(rgb[y*(width*3) + x*3 + 1]);
            data.push_back(rgb[y*(width*3) + x*3 + 0]);
        }
        while ((data.size() - size_of_header)%4)
        {
            data.push_back(0);
        }
    }
    {
        uint32_t file_size = data.size();
        memcpy(&data[file_size_offset], &file_size, 4);
    }
    {
        uint32_t pixel_data_size = data.size() - size_of_header;
        memcpy(&data[raw_pixel_array_size_offset], &pixel_data_size, 4);
    }
    *output = new uint8_t[data.size()];
    memcpy(*output, &data[0], data.size());
    return data.size();
}

// Added by Jarno N. Alanko on 12.16.2020
// Reads a BMP file encoded in bmp. Stores the width and the
// height to the given parameters and returns the pixel data in
// row-major order, 3 components per pixel. The caller is responsible
// for freeing the pixel data later. Coordinate (0,0) is the bottom
// left corner.
uint8_t* bitmap_decode_rgb(uint8_t* bmp, long long* width, long long* height){
    assert(bmp[0] == 'B');
    assert(bmp[1] == 'M');
    //LL size = parse_int(bmp + 2);
    long long data_start = parse_int(bmp + 2 + 4 + 4);
    *width = parse_int(bmp + 2 + 4 + 4 + 4 + 4);
    *height = parse_int(bmp + 2 + 4 + 4 + 4 + 4 + 4);

    uint8_t* pixels = (uint8_t*)malloc((*width) * (*height)*3);

    long long data_idx = data_start;
    long long pix_idx = 0;
    for(long long r = 0; r < *height; r++){
        for(long long c = 0; c < *width; c++){
            // BMP uses BGR color order whereas we use RGB
            pixels[pix_idx+0] = bmp[data_idx+2];
            pixels[pix_idx+1] = bmp[data_idx+1];
            pixels[pix_idx+2] = bmp[data_idx+0];
            pix_idx += 3;
            data_idx += 3;
        }
        while((data_idx - data_start) % 4 != 0){
            data_idx++; // Rows are padded to align with multiples of four
        }
    }

    return pixels;
}

