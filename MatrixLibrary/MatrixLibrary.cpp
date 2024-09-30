#include "pch.h"
#include "MatrixLibrary.h"

double* matrix_multiply_2d_worker(int nrows, int ncols, double* n, int mrows, int mcols, double* m, double* ret);
double* generate_matrix_slice_vertical(int nrows, int ncols, double* n, int scols, int startCol);
double* generate_matrix_slice_horizontal(int mrows, int mcols, double* m, int srows, int startRow);


double* matrix_multiply_2d(int nrows, int ncols, double* n, int mrows, int mcols, double* m)
{
	if (ncols != mrows) return 0;

	int finalNumRows = nrows;
	int finalNumCols = mcols;
	int numElements = finalNumRows * finalNumCols;
	double* ret = new double[numElements];
	for (int i = 0; i < numElements; i++) { *(ret + i) = 0; }//Default initialize ret to zero
	//The goal is to create a bunch of matrix slices that all can hit the optimized case where there's only one loop
	//This will make matrix multiplication as fast as I could imagine here
	if (ncols > 5 && numElements > 100 * 100) 
	{

		int numSlices = ncols / 5;
		int numColsOnLast = ncols % 5;
		std::thread* threads = new std::thread[numSlices];
		int startIndex = 0;
		for (int i = 0; i < numSlices; i++)
		{
			int sliceSize = 5;
			if (i + 1 == numSlices) //ncols = mrows so this works for n and m
			{
				sliceSize = numColsOnLast;
			}
			double* nslice = generate_matrix_slice_vertical(nrows, ncols, n, sliceSize, i * 5); //Generate a vertical slice of n
			double* mslice = generate_matrix_slice_horizontal(mrows, mcols, m, sliceSize, i * 5); //Generate a horizontal slice of m

			// Create a thread that runs the threadFunction
			matrix_multiply_2d_worker(nrows, sliceSize, nslice, sliceSize, mcols, mslice, ret);
		}	
	}
}

double* generate_matrix_slice_vertical(int nrows, int ncols, double* n, int scols, int startCol)
{
	int numElements = nrows * scols;
	double* slice = new double[numElements];
	for (int i = 0; i < numElements; i++)
	{
		//scols = 5
		// n
		//  0  1  2  3  4  5  6
		//  7  8  9 10 11 12 13
		// 14 15 16 17 18 19 20
		// 21 22 23 24 25 26 27
		//slice
		//  0  1  2  3  4  0  0
		//  5  6  7  8  9  0  0
		// 10 11 12 13 14  0  0
		// 15 16 17 18 19  0  0

		int nrow = i / scols; //First ncols index should be 0, next ncols 1, etc
		int ncol = i % scols; //0,1,2,3,4,0,1,2,3,4 if scols = 5
		*(slice + i) = *(n + (nrow * ncols) + ncol + startCol); //Start col is just a shift to the right on each index
	}
	return slice;
}

double* generate_matrix_slice_horizontal(int mrows, int mcols, double* m, int srows, int startRow)
{
	int numElements = mcols * srows;
	double* slice = new double[numElements];
	for (int i = 0; i < numElements; i++)
	{
		//scols = 5
		// n
		//  0  1  2  3  4  5  6
		//  7  8  9 10 11 12 13
		// 14 15 16 17 18 19 20
		// 21 22 23 24 25 26 27
		// 28 29 30 31 32 33 34
		// 35 36 37 38 39 40 41
		//slice
		//  0  1  2  3  4  5  6
		//  7  8  9 10 11 12 13
		// 14 15 16 17 18 19 20
		// 21 22 23 24 25 26 27
		// 28 29 30 31 32 33 34
		//  0  0  0  0  0  0  0

		//This is just gathering, in direct order, numElements elements and putting them in the slice
		*(slice + i) = *(m + (startRow * mcols) + i);
	}
	return slice;
}

double* matrix_multiply_2d_worker(int nrows, int ncols, double* n, int mrows, int mcols, double* m, double* ret)
{
	if (ncols != mrows) return 0;

	int finalNumRows = nrows;
	int finalNumCols = mcols;
	int numElements = finalNumRows * finalNumCols;
	if (numElements == 0) return 0;

	switch (ncols) //Handle these cases (its faster if we catch)
	{
	case 1:
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			*(ret + i) += *(n + (nrow * ncols)) * *(m + mcol); //Just multiply the indices together
		}
		break;
	case 2:
		// [2 2] * [ 1 ] = [ 2*1 + 2*1 ]
		//         [ 1 ]   [ 2*1 + 2*1 ]
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			//These make this code much more readable
			//Hoping the compiler optimizes them out of the actual code though
			double* n1 = (n + (nrow * ncols));
			double* n2 = (n1 + 1);

			double* m1 = (m + mcol);
			double* m2 = (m1 + mcols);

			*(ret + i) += ((*n1 * *m1) + (*n2 * *m2));
		}
		break;
	case 3:
		// [2 1 2] * [ 1 ] = [ 2*1 + 1*1 + 2*1 ]
		//           [ 2 ]   [ 2*2 + 1*2 + 2*1 ]
		//           [ 1 ]   [ 2*1 + 1*1 + 2*1 ]
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			double* n1 = (n + (nrow * ncols));
			double* n2 = (n1 + 1);
			double* n3 = (n2 + 1);

			double* m1 = (m + mcol);
			double* m2 = (m1 + mcols);
			double* m3 = (m2 + mcols);

			*(ret + i) += ((*n1 * *m1) + (*n2 * *m2) + (*n3 * *m3));
			//printf("%d * %d + %d * %d + %d * %d = %d\n", *n1, *m1, *n2, *m2, *n3, *m3, *(ret + i));
		}
		break;
	case 4:
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			double* n1 = (n + (nrow * ncols));
			double* n2 = (n1 + 1);
			double* n3 = (n2 + 1);
			double* n4 = (n3 + 1);

			double* m1 = (m + mcol);
			double* m2 = (m1 + mcols);
			double* m3 = (m2 + mcols);
			double* m4 = (m3 + mcols);

			*(ret + i) += ((*n1 * *m1) + (*n2 * *m2) + (*n3 * *m3) + (*n4 * *m4));
			//printf("%d * %d + %d * %d + %d * %d + %d * %d = %d\n", *n1, *m1, *n2, *m2, *n3, *m3, *n4, *m4, *(ret + i));
		}
		break;
	case 5:
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			double* n1 = (n + (nrow * ncols));
			double* n2 = (n1 + 1);
			double* n3 = (n2 + 1);
			double* n4 = (n3 + 1);
			double* n5 = (n4 + 1);

			double* m1 = (m + mcol);
			double* m2 = (m1 + mcols);
			double* m3 = (m2 + mcols);
			double* m4 = (m3 + mcols);
			double* m5 = (m4 + mcols);

			*(ret + i) += ((*n1 * *m1) + (*n2 * *m2) + (*n3 * *m3) + (*n4 * *m4) + (*n5 * *m5));
		}

		break;
	default:
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			for (int j = 0; j < ncols; j++) //The extra loop makes it woefully slower but it works for all cases, so oh well
			{
				double* nj = (n + (nrow * ncols) + j);
				double* mj = (m + mcol + (mcols * j));
				*(ret + i) += *nj * *mj;
			}
		}
		break;
	}
	return ret;
}

