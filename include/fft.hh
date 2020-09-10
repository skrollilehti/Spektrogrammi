#pragma once

// These are taken with permission from https://github.com/anroysko/contestlib/blob/master/src/math/fft.cpp

using namespace std;
const double PI=atan2((double)0.0, (double)-1.0);

// Applies the bit-reverse permutation to the given vector
// Preconditions: vec.size() = 2^lg for some lg.
// Time Complexity: O(n log n)
template<class T>
void bitReverse(vector<T>& vec) {
	int n = vec.size();
	int lg = 0;
	while((1<<lg) < n) ++lg;
	for (int i = 0; i < n; ++i) {
		int t = 0;
		for (int j = 0; j < lg; ++j) {
			if (i & (1<<j)) t |= 1<<(lg-1-j);
		}
		if (i < t) swap(vec[i], vec[t]);
	}
}

// Iterative FFT.
// Preconditions: pol has length that is a power of two
// Time Complexity: O(n log(n))
void fft_in_place(vector<complex<double>>& pol) {
	bitReverse(pol); // Lets us calculate FFT iteratively

	int n = pol.size();
	int lg = 0;
	while((1<<lg) < n) ++lg;
	
    complex<double> nth_root = polar(1.0, 2*PI / n);
	vector<complex<double>> pows(lg); // nth_root^(2^(lg-1-i))
	pows.back() = nth_root;
	for (int i = lg-2; i >= 0; --i) pows[i] = pows[i+1] * pows[i+1];
	
	for (lg = 0; (1<<lg) < n; ++lg) {
		int h = 1<<lg;	// half
		int len = h<<1;	// step length
		complex<double> root = pows[lg];
		for (int j = 0; j < n; j += len) {
			complex<double> curr = 1;
			for (int i = j; i < j+h; ++i) {
				auto tmp = curr * pol[i + h];
				pol[i + h] = pol[i] -  tmp;
				pol[i] += tmp;
				curr *= root;
			}
		}
	}
}

// Input must have a length that is a power of two
vector<complex<double> > fft(const vector<complex<double> >& input){
    assert(1 << (int)log2(input.size()) == input.size()); // Check that the length is a power of two
    vector<complex<double> > output = input;
    fft_in_place(output);
    return output;
}

// Input must have a length that is a power of two
vector<complex<double> > fft(const vector<double>& input){
    assert(1 << (int)log2(input.size()) == input.size()); // Check that the length is a power of two
    vector<complex<double> > output;
	for(double x : input) output.push_back(complex<double>(x,0));
    fft_in_place(output);
    return output;
}