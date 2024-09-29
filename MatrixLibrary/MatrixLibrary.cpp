#include "pch.h"
#include "MatrixLibrary.h"

UINT_32* matrix_multiply_2d(int nrows, int ncols , UINT_32* n, int mrows, int mcols, UINT_32* m)
{
	if (ncols != mrows) return 0;

	int finalNumRows = nrows;
	int finalNumCols = mcols;
	int numElements = finalNumRows * finalNumCols;
	UINT_32* ret = new UINT_32[numElements];
	if (numElements == 0) return 0;

	switch (ncols) //Handle these cases (its faster if we catch)
	{
	case 1:
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			*(ret + i) = *(n + (nrow * ncols)) * *(m + mcol); //Just multiply the indices together
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
			UINT_32* n1 = (n + (nrow * ncols));
			UINT_32* n2 = (n1 + 1);

			UINT_32* m1 = (m + mcol);
			UINT_32* m2 = (m1 + mcols);

			*(ret + i) = ((*n1 * *m1) + (*n2 * *m2));
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
			UINT_32* n1 = (n + (nrow * ncols));
			UINT_32* n2 = (n1 + 1);
			UINT_32* n3 = (n2 + 1);

			UINT_32* m1 = (m + mcol);
			UINT_32* m2 = (m1 + mcols);
			UINT_32* m3 = (m2 + mcols);

			*(ret + i) = ((*n1 * *m1) + (*n2 * *m2) + (*n3 * *m3));
			printf("%d * %d + %d * %d + %d * %d = %d\n", *n1, *m1, *n2, *m2, *n3, *m3,  *(ret + i));
		}
		break;
	case 4:
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			UINT_32* n1 = (n + (nrow * ncols));
			UINT_32* n2 = (n1 + 1);
			UINT_32* n3 = (n2 + 1);
			UINT_32* n4 = (n3 + 1);

			UINT_32* m1 = (m + mcol);
			UINT_32* m2 = (m1 + mcols);
			UINT_32* m3 = (m2 + mcols);
			UINT_32* m4 = (m3 + mcols);

			*(ret + i) = ((*n1 * *m1) + (*n2 * *m2) + (*n3 * *m3) + (*n4 * *m4));
			printf("%d * %d + %d * %d + %d * %d + %d * %d = %d\n", *n1, *m1, *n2, *m2, *n3, *m3, *n4, *m4, *(ret + i));
		}
		break;
	case 5:
		for (int i = 0; i < numElements; i++)
		{
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			UINT_32* n1 = (n + (nrow * ncols));
			UINT_32* n2 = (n1 + 1);
			UINT_32* n3 = (n2 + 1);
			UINT_32* n4 = (n3 + 1);
			UINT_32* n5 = (n4 + 1);

			UINT_32* m1 = (m + mcol);
			UINT_32* m2 = (m1 + mcols);
			UINT_32* m3 = (m2 + mcols);
			UINT_32* m4 = (m3 + mcols);
			UINT_32* m5 = (m4 + mcols);

			*(ret + i) = ((*n1 * *m1) + (*n2 * *m2) + (*n3 * *m3) + (*n4 * *m4) + (*n5 * *m5));
		}
		
		break;
	default:
		if (numElements > 10000)
		{

		}
		else //Single threaded case
		{
			for (int i = 0; i < numElements; i++)
			{
				*(ret + i) = 0;
				int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
				int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
				for (int j = 0; j < ncols; j++) //The extra loop makes it woefully slower but it works for all cases, so oh well
				{
					UINT_32* nj = (n + (nrow * ncols) + j);
					UINT_32* mj = (m + mcol + (mcols * j));
					*(ret + i) += *nj * *mj;
				}
			}
		}
		break;
	}
	return ret;
}

void multithreaded(int start, int stop, int mcols, int ncols, UINT_32* ret, UINT_32* n, UINT_32* m)
{
	for (int i = start; i < stop; i++)
	{
		for (int i = 0; i < stop; i++)
		{
			*(ret + i) = 0;
			int nrow = i / mcols; //First ncols index should be 0, next ncols 1, etc
			int mcol = i % mcols; //I.E. 0,1,2,0,1,2 if mcols is 3
			for (int j = 0; j < ncols; j++) //The extra loop makes it woefully slower but it works for all cases, so oh well
			{
				UINT_32* nj = (n + (nrow * ncols) + j);
				UINT_32* mj = (m + mcol + (mcols * j));
				*(ret + i) += *nj * *mj;
			}
		}
	}
}