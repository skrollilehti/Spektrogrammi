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

vector<double> trianlugar_window(vector<double> v){
    vector<double> v2 = v;
    for(LL i = 0; i < v2.size(); i++){
        double dist_to_end = min((LL)i, (LL)(v2.size()-1 - i));
        //if(dist_to_end <= 500) v2[i] *= dist_to_end / 500.0;
        v2[i] *= dist_to_end / (v2.size()/2.0);
    }
    return v2;
}


vector<double> hann_window(vector<double> v){
    vector<double> v2 = v;
    for(LL i = 0; i < v2.size(); i++){
        double t = (double)i / (v2.size()-1);
        v2[i] *= pow(sin(t * PI), 2);
    }
    return v2;
}
