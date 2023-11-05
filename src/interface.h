
#pragma once

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "matrix.h"


#include <iostream>


void copy2DNumpyArray(pybind11::array_t<double> x);
void copy3DNumpyArray(pybind11::array_t<double> x);

inline Matrix2D* myVec2D = nullptr;
inline Matrix3D* myVec3D = nullptr;