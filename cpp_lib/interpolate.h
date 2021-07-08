//
// Created by ZHENG FENG on 7/7/21.
//

#ifndef FENG_INTERPOLATE_H
#define FENG_INTERPOLATE_H
#include <vector>
using namespace std;

class interpolate {
    vector<float> x;
    vector<float> y;
    vector<float> a, b, c, d; // cubic splines coefficients
    string interpolate_type, extrapolate_type;
public:
    interpolate(vector<float> vx, vector<float> vy,string interpolate_in, string extrapolate_in);
    float interp(float xx);
};
#endif //FENG_INTERPOLATE_H
