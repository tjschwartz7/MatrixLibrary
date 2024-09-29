#pragma once
#include "iostream"

#ifdef MATRIX_LIBRARY_EXPORTS
#define MATRIX_LIBRARY_API __declspec(dllexport)
#else
#define MATRIX_LIBRARY_API __declspec(dllimport)
#endif
#define UINT_32 unsigned int

extern "C" MATRIX_LIBRARY_API UINT_32 * matrix_multiply_2d(int n1, int n2, UINT_32 * n, int m1, int m2, UINT_32 * m);