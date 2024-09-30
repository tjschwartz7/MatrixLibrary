#pragma once
#include "iostream"
#include <thread>

#ifdef MATRIX_LIBRARY_EXPORTS
#define MATRIX_LIBRARY_API __declspec(dllexport)
#else
#define MATRIX_LIBRARY_API __declspec(dllimport)
#endif
#define UINT_32 unsigned int

extern "C" MATRIX_LIBRARY_API double * matrix_multiply_2d(int n1, int n2, double* n, int m1, int m2, double* m);