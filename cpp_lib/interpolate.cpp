#include <iostream>
#include <vector>
#include "Eigen/Dense"
#include "interpolate.h"

using namespace Eigen;
using namespace std;

interpolate::interpolate(vector<float> vx, vector<float> vy,string interpolate_in, string extrapolate_in) {
        interpolate_type = interpolate_in;
        extrapolate_type = extrapolate_in;
        x = vx;
        y = vy;

        if (interpolate_type != "linear" && interpolate_type != "cubic") {
            throw invalid_argument("interpolate_type must be either \"linear\" or \"cubic\"");
        }
        if (extrapolate_type != "linear" && extrapolate_type != "flat") {
            throw invalid_argument("extrapolate_type must be either \"linear\" or \"flat\"");
        }
        if (x.size() != y.size()) {
            throw invalid_argument("length of vector x and y must be equal");
        }
        if (x.size() < 2) {
            throw invalid_argument("length of vector x or y must be not smaller than 2");
        }

        // calculate diff on x and y vectors and store in dx and dy
        vector<double> dx;
        vector<double> dy;
        int size = x.size();
        for (int i = 0; i < size - 1; i++) {
            double temp_dx = x[i + 1] - x[i];
            dx.push_back(temp_dx);
            double temp_dy = y[i + 1] - y[i];
            dy.push_back(temp_dy);
        }

        // calculate coefficients of cubic splines
        if (interpolate_type == "cubic")
        {
            MatrixXd H = MatrixXd::Random(size, size);
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    H(i, j) = 0;
                }
            }
            VectorXd Y(size);
            for (int i = 0; i < size; i++) {
                Y(i) = 0;
            }
            VectorXd M(size);
            for (int i = 0; i < size; i++) {
                M(i) = 0;
            }

            // Solve M from HM = Y by using QR decomposition
            H(0, 0) = 1;
            H(size - 1, size - 1) = 1;
            for (int i = 1; i < size - 1; i++) {
                H(i, i - 1) = dx[i - 1];
                H(i, i) = 2 * (dx[i - 1] + dx[i]);
                H(i, i + 1) = dx[i];
                Y(i) = 3 * (dy[i] / dx[i] - dy[i - 1] / dx[i - 1]);
            }
            M = H.colPivHouseholderQr().solve(Y);

            for (int i = 0; i < size - 1; i++) {
                a.push_back(y[i]);
                d.push_back((M(i + 1) - M(i)) / (3 * dx[i]));
                b.push_back(dy[i] / dx[i] - dx[i] * (2 * M(i) + M(i + 1)) / 3);
                c.push_back(M(i));
            }
        }
    }
float interpolate::interp(float xx) {
        // find out [x_{i}, x_{i+1}) interval xx is in
        int size = x.size();
        int i = 0;
        if (xx > x[size - 2])
        {
            i = size - 2;
        }
        else
        {
            while (xx > x[i + 1]) i++;
        }
        float xL = x[i], yL = y[i], xR = x[i+1], yR = y[i+1];

        // linear splines
        if (interpolate_type == "linear")
        {
            if (extrapolate_type == "flat")
            {
                if (xx < xL) yR = yL;
                if (xx > xR) yL = yR;
            }

            return yL + (yL - yR) / (xL - xR) * (xx - xL);
        }

        // cubic splines
        if (interpolate_type == "cubic")
        {
            if (xx < xL) {
                if (extrapolate_type == "flat") return yL;
                if (extrapolate_type == "linear") {
                    return yL + (b[i] + 2 * c[i] * (xL - x[i]) + 3 * d[i] * pow((xL - x[i]), 2)) * (xx - xL);
                }
            }
            if (xx > xR) {
                if (extrapolate_type == "flat") return yR;
                if (extrapolate_type == "linear") {
                    return yR + (b[i] + 2 * c[i] * (xR - x[i]) + 3 * d[i] * pow((xR - x[i]), 2)) * (xx - xR);
                }
            }
            return a[i] + b[i] * (xx - x[i]) + c[i] * pow((xx - x[i]), 2) + d[i] * pow((xx - x[i]), 3);
        }

    }

extern "C"
{
interpolate* interpolate_new(float* ptr_x, int size_x, float* ptr_y, int size_y
                             ,char* interp, int len_interp, char* extrap, int len_extrap)
{   vector<float> vx(ptr_x, ptr_x + size_x);
    vector<float> vy(ptr_y, ptr_y + size_y);
    string interpolate_in(interp, interp + len_interp);
    string extrapolate_in(extrap, extrap + len_extrap);
    return new interpolate(vx,vy, interpolate_in, extrapolate_in);
}

float interpolate_interp(interpolate* fun, float xx) {return fun->interp(xx);}
}
