#include "matrix_operations.h"

void determinant(float *A, float *d, int *exp, int matrixSize)
{
	*d = 1.0;
	*exp = 0;

	// Compute gaussian elimination
	for (int pivot = 0; pivot < matrixSize - 1; pivot++)
	{
		// FOR can be parallelized
		for (int y = pivot + 1; y < matrixSize; y++)
		{
			float multiplier = A[y*matrixSize + pivot] / A[pivot*matrixSize + pivot];

			// multiply and substract "pivot" row from "y" row -- FOR can be parallelized
			for (int x = pivot + 1; x < matrixSize; x++)
			{
				A[y*matrixSize + x] -= (multiplier * A[pivot*matrixSize + x]);
			}

			// Add zero to create upper triangular matrix - useless
			A[y*matrixSize + pivot] = 0;
		}
	}

	// Compute determinant from main diagonal
	for (int i = 0; i < matrixSize; i++)
	{
		*d = *d * A[i*matrixSize + i];

		// Prevent infinity result
		if (*d > 1000000 || *d < -1000000)
		{
			exp += 6;
			*d /= 1000000;
		}
	}
}

void gem(float *A, float *result, int matrixSize)
{
	for (int i = 0; i < matrixSize; i++)
	{
		result[i] = 1;
	}

	// Compute gaussian elimination
	for (int pivot = 0; pivot < matrixSize; pivot++)
	{
		float divider = A[pivot*matrixSize + pivot];
		for (int x = pivot; x < matrixSize; x++)
		{
			A[pivot*matrixSize + x] /= divider;
		}
		result[pivot] /= divider;

		// FOR can be parallelized
		for (int y = 0; y < matrixSize; y++)
		{
			if (y != pivot)
			{
				float multiplier = A[y*matrixSize + pivot] / A[pivot*matrixSize + pivot];

				// multiply and substract "pivot" row from "y" row -- FOR can be parallelized
				for (int x = pivot + 1; x < matrixSize; x++)
				{
					A[y*matrixSize + x] -= multiplier * A[pivot*matrixSize + x];
				}
				result[y] -= multiplier*result[pivot];

				// Add zero to create upper triangular matrix - useless
				A[y*matrixSize + pivot] = 0;
			}
		}
	}
}

void inverse(float *A, float *result, int matrixSize)
{
	// Compute gaussian elimination
	for (int pivot = 0; pivot < matrixSize; pivot++)
	{
		float divider = A[pivot*matrixSize + pivot];
		for (int x = pivot; x < matrixSize; x++)
		{
			A[pivot*matrixSize + x] /= divider;
		}
		for (int x = 0; x < matrixSize; x++)
		{
			result[pivot*matrixSize + x] /= divider;
		}

		// FOR can be parallelized
		for (int y = 0; y < matrixSize; y++)
		{
			if (y != pivot)
			{
				float multiplier = A[y*matrixSize + pivot] / A[pivot*matrixSize + pivot];

				// multiply and substract "pivot" row from "y" row -- FOR can be parallelized
				for (int x = pivot + 1; x < matrixSize; x++)
				{
					A[y*matrixSize + x] -= multiplier * A[pivot*matrixSize + x];
				}

				for (int x = 0; x < matrixSize; x++)
				{
					result[y*matrixSize + x] -= multiplier * result[pivot*matrixSize + x];
				}

				// Add zero to create upper triangular matrix - useless
				A[y*matrixSize + pivot] = 0;
			}
		}
	}
}