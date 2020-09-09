#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <complex>
#include <cmath>
#include "readWav.hh"
#include "bitmap.hh"
#include "cxxopts.hpp"
#include "fft.hh"

using namespace std;

vector<double> hann_window(const vector<double>& v){
    vector<double> v2 = v;
    for(int i = 0; i < v2.size(); i++){
        double t = (double)i / (v2.size()-1);
        v2[i] *= pow(sin(t * PI), 2);
    }
    return v2;
}

struct Pixel{
    uint8_t red, green, blue; // Punainen, vihreä ja sininen komponentti
    Pixel() : red(0), green(0), blue(0) {}
};

// Palauttaa spektrogrammin S, jossa S[i][j] on kuvan rivin i sarakkeen j pikseli
vector<vector<Pixel> > build_spectrogram(
                int nrows, int ncols, double dB_floor, double dB_ceil, 
                const vector<double>& samples, double glow_threshold){

    vector<vector<Pixel> > image(nrows, vector<Pixel>(ncols)); // Kuvan mitat: nrows * ncols
    int window_size = nrows; // Ikkunan koko näytteissä
    double max_intensity = 0; // Suurin intensiteetti koko spektrogrammissa
    for(int column = 0; column < ncols; column++){
        int start = column * ((samples.size()-window_size)/ncols); // Ikkunan alku
        int end = min(start + window_size, (int)samples.size()-1); // Yksi ikkunan yli lopun
        vector<double> window_samples(samples.begin() + start, samples.begin() + end);
        vector<complex<double> > DFT = fft(hann_window(window_samples));
        for(int j = 0; j < window_size; j++){
            double log_amplitude = log10(abs(DFT[j]) / window_size); // Välillä (-ääretön,0]
            double intensity = min(dB_ceil, max(dB_floor, log_amplitude)) - dB_floor; // Välillä [0, dB_ceiling-dB_floor]
            intensity /= dB_ceil - dB_floor; // Välillä [0,1]
            image[j][column].red = intensity * 255; // Aseta punainen värikomponentti
            max_intensity = max(intensity, max_intensity); // Pidä kirjaa maksimista
        }
    }

    // Normalisoi ja lisää vihreät ja siniset komponentit
    for(int i = 0; i < nrows; i++){
        for(int j = 0; j < ncols; j++) {
            image[i][j].red /= max_intensity; // Normalisoi maksimi ykköseen
            double d = image[i][j].red / 255.0; // Välillä [0,1]
            if(d > glow_threshold){
                image[i][j].green = (d-glow_threshold)/(1 - glow_threshold) * 255;
                image[i][j].blue =  (d-glow_threshold)/(1 - glow_threshold) * 255;
            }
        }
    }
    return image;
}

void create_image(vector<vector<Pixel> > spectrogram, 
                  double fmin, double fmax, int sr,
                  string outfile){


    assert(fmin < fmax);

    int nrows = spectrogram.size();
    int ncols = spectrogram[0].size();

    double freq_bin_size = sr / nrows; // How much frequency each row in the spectrogram covers

    cerr << "Frequency bin size: " << freq_bin_size << endl;
    int max_row = min((int)(fmax / freq_bin_size), nrows-1);
    int min_row = max((int)(fmin / freq_bin_size), 0);

    cerr << "Saving to: " << outfile << endl;
    cerr << "Output image width x height: " << ncols << " " << max_row-min_row+1 << endl;

    // Take the rows between min_row and max_row
    vector<uint8_t> pixels(nrows * ncols * 3);
    for(int row = min_row; row <= max_row; row++){
        for(int col = 0; col < ncols; col++){
            pixels[row*(ncols*3) + col*3 + 0] = spectrogram[row][col].red;
            pixels[row*(ncols*3) + col*3 + 1] = spectrogram[row][col].green;
            pixels[row*(ncols*3) + col*3 + 2] = spectrogram[row][col].blue;
        }
    }
    
    // Converto to bmp format
    uint8_t* bmp_output;
    int output_size = bitmap_encode_rgb(pixels.data(), ncols, nrows, &bmp_output);

    // Write to file
    std::ofstream file_output;
    file_output.open(outfile);
    file_output.write((const char*)bmp_output, output_size);
    file_output.close();
    delete [] bmp_output;
}


int main(int argc, char** argv){

    cxxopts::Options options("Spectrogram", "Takes a wav-file and creates a bmp-image");
    int original_argc = argc; // It seems the CLI parsing library modifies argc, so store the original value

    options.add_options()
      ("w,width", "Image width", cxxopts::value<int>()->default_value("1920"))
      //("h,height", "Image height", cxxopts::value<int>()->default_value("1080"))
      ("r,resolution", "SFTP window size (must be a power of 2). A window of size r samples r/2 different frequencies", cxxopts::value<int>()->default_value("4096"))
      ("s,start", "Start time in seconds", cxxopts::value<double>()->default_value("0"))
      ("e,end", "End time in seconds", cxxopts::value<double>()->default_value("1000000000"))
      ("db-floor", "Only frequencies louder than this are drawn (dB)", cxxopts::value<double>()->default_value("-8"))
      ("db-ceil", "Frequencies louder than this are clamped to this value (dB)", cxxopts::value<double>()->default_value("-5"))
      ("fmin", "Crop minimum frequency to be shown on the image", cxxopts::value<double>()->default_value("0"))
      ("fmax", "Crop maximum frequency to be shown on the image", cxxopts::value<double>()->default_value("22050"))
      ("i,input", "Input wav-file with 44100Hz sample rate",cxxopts::value<string>())
      ("o,output", "Output bmp file",cxxopts::value<string>()->default_value("out.bmp"))
      ("help", "Print usage instructions", cxxopts::value<bool>()->default_value("false"))
    ;

    auto cli_params = options.parse(argc, argv);

    if (cli_params["help"].as<bool>() == true || original_argc == 1){
        cerr << options.help() << endl;
        return 1;
    }

    int nrows = cli_params["resolution"].as<int>();
    int ncols = cli_params["width"].as<int>();

    double fmin = cli_params["fmin"].as<double>();
    double fmax = cli_params["fmax"].as<double>();

    auto is_power_of_two = [](int x){
        int ones = 0;
        while(x != 0){
            if(x & 1) ones++;
            x >>= 1;
        }
        return ones == 1;
    };

    if(!is_power_of_two(nrows)){
        cerr << "Error: resolution " << nrows << " is not a power of two" << endl;
        return 1;
    }

    double dB_floor = cli_params["db-floor"].as<double>();
    double dB_ceiling = cli_params["db-ceil"].as<double>();

    vector<short> v_short; int sr;
    std::tie(v_short, sr) = readUncompressedWavFile(cli_params["input"].as<string>());
    string outfile = cli_params["output"].as<string>();

    double start_time = cli_params["start"].as<double>();
    start_time = max(0.0, start_time);
    
    double end_time = cli_params["end"].as<double>();
    end_time = min(end_time, (double)v_short.size() / sr); // If end-time is past the end of the audio, set it to the end of the song

    auto normalize_to_plusminus_1 = [](vector<short>& v){
        int minv = *std::min_element(v.begin(), v.end());
        int maxv = *std::max_element(v.begin(), v.end());
        double scaling = max(abs(minv), abs(maxv));
        vector<double> ans;
        for(short x : v) ans.push_back(x/scaling);
        return ans;
    };

    vector<double> samples = normalize_to_plusminus_1(v_short);
    vector<vector<Pixel> > spectrogram = build_spectrogram(nrows, ncols, dB_floor, dB_ceiling, samples, 0.5);

    create_image(spectrogram, fmin, fmax, sr, outfile);

}