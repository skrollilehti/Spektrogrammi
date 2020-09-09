#pragma once

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include "globals.hh"
#include "readWav.hh"
#include "fft.hh"
#include "bitmap.hh"

//vector<double> get_intensities(const vector<double>& window){
//
//}

bool is_power_of_two(LL x){
    LL ones = 0;
    while(x != 0){
        if(x & 1) ones++;
        x >>= 1;
    }
    return ones == 1;
}

vector<double> normalize_to_plusminus_1(vector<short>& v){
    LL minv = *std::min_element(v.begin(), v.end());
    LL maxv = *std::max_element(v.begin(), v.end());
    double scaling = max(abs(minv), abs(maxv));
    vector<double> ans;
    for(short x : v) ans.push_back(x/scaling);
    return ans;
}

vector<complex<double> > to_complex(vector<double>& v){
    vector<complex<double> > ans;
    for(double x : v) ans.push_back(complex<double>(x,0)); // Imaginary part 0
    return ans;
}


// pix_buf contains the red, green, blue values of all pixels, in row-major order, pixel components last
void save_to_bmp(uint8_t* pix_buf, LL nrows, LL ncols, string outfile){

    // Converto to bmp format
    uint8_t* bmp_output;
    int output_size = bitmap_encode_rgb(pix_buf, ncols, nrows, &bmp_output);

    // Write to file
    std::ofstream file_output;
    file_output.open(outfile);
    file_output.write((const char*)bmp_output, output_size);
    file_output.close();
    delete [] bmp_output;
}


// RGB contains the red, green, blue values of all pixels, in row-major order, pixel components last
void save_to_bmp(vector<vector<vector<uint8_t > > >& RGB, string outfile){

    LL nrows = RGB.size();
    LL ncols = RGB[0].size();

    // Arrange the pixels like the bmp encoder wants them
    uint8_t* pix_buf = (uint8_t*)malloc(sizeof(uint8_t) * nrows * ncols * 3);
    for(LL r = 0; r < nrows; r++){
        for(LL c = 0; c < ncols; c++){
            pix_buf[r*(ncols*3) + c*3 + 0] = RGB[r][c][0];
            pix_buf[r*(ncols*3) + c*3 + 1] = RGB[r][c][1];
            pix_buf[r*(ncols*3) + c*3 + 2] = RGB[r][c][2];
        }
    }

    save_to_bmp(pix_buf, nrows, ncols, outfile);
    free(pix_buf);

}

// Creates a vector of n evenly spaced numbers between start and end
// such that the first number is start and the last number is end.
// Precondition: n >= 2
vector<double> linspace(double start, double end, LL n){
    assert(n >= 2);
    // The points divide [start,end] to n-1 segments.
    double seg_length = (end-start) / (n-1);
    vector<double> ans = {start};
    for(LL i = 0; i < n-1; i++){
        ans.push_back(ans.back() + seg_length);
    }
    return ans;
}

// Multiplies the number of samples by the given factor. New
// samples are computed with linear interpolation.
template<typename T>
vector<T> supersample(vector<T> v, LL factor){
    vector<T> v2;
    for(LL i = 0; i < v.size(); i++){
        T x1 = v[i];
        T x2 = i == v.size()-1 ? v[i] : v[i+1];
        for(LL j = 0; j < factor; j++){
            double t = (double)j / factor;
            v2.push_back(x1 + (x2-x1)*t);
        }
    }
    return v2;
}

template<typename T>
vector<T> self_concatenate(vector<T> v, LL times){
    vector<T> u;
    for(LL i = 0; i < times; i++){
        for(T x : v) u.push_back(x);
    }
    return u;
}

template<typename T>
vector<T> zero_pad(vector<T> v, LL zeroes){
    vector<T> u = v;
    for(LL i = 0; i < zeroes; i++){
        u.push_back(0);
    }
    return u;
}