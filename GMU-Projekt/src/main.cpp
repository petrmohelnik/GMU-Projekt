#include "CL\opencl.h"
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>

//#define DEBUG

using namespace std;

const int COUT_NUMBER_WIDTH = 7;
const int COUT_NUMBER_PRECISION = 3;
const int MATRIX_SIZE = 1 << 10;

void cpu_determinant(long double *A, int matrixSize)
{
	long double d = 1;
	int exp = 0;

	clock_t begin = clock();

	// Compute gaussian elimination
	for (int pivot = 0; pivot < matrixSize - 1; pivot++)
	{
		if (A[pivot*matrixSize + pivot] == 0)
		{
			// find non zero row
			int sy;
			for (sy = pivot + 1; sy < matrixSize; sy++)
			{
				if (A[sy*matrixSize + pivot] != 0)
				{
					break;
				}
			}

			// swap lines -- FOR can be parallelized
			for (int sx = pivot; sx < matrixSize; sx++)
			{
				long double tmp = A[pivot*matrixSize + sx];
				A[pivot*matrixSize + sx] = A[sy*matrixSize + sx];
				A[sy*matrixSize + sx] = tmp;
			}
			d *= -1;
		}
		// FOR can be parallelized
		for (int y = pivot + 1; y < matrixSize; y++)
		{
			long double multiplier = A[y*matrixSize + pivot] / A[pivot*matrixSize + pivot];

			// multiply and substract "pivot" row from "y" row -- FOR can be parallelized
			for (int x = pivot + 1; x < matrixSize; x++)
			{
				A[y*matrixSize + x] -= (multiplier * A[pivot*matrixSize + x]);
			}

			// Add zero to create upper triangular matrix - useless
			A[y*matrixSize + pivot] = 0;
		}
	}

	//cout << float(clock() - begin) << " ms" << endl;

	// Compute determinant from main diagonal
	for (int i = 0; i < matrixSize; i++)
	{
		d = d * A[i*matrixSize + i];

		// Prevent infinity result
		if (d > 1000000 || d < -1000000)
		{
			exp += 6;
			d /= 1000000;
		}
	}

	cout << "Determinant: " << d;
	if (exp > 0)
	{
		cout << "e+" << exp;
	}
	cout << endl;
}

int main(int argc, char* argv[])
{
	cl_uint platformsCount;
	cl_platform_id *platform;
	cl_device_id gpuDevice;

	clGetPlatformIDs(0, NULL, &platformsCount);
	platform = (cl_platform_id *)malloc(platformsCount * sizeof(cl_platform_id));
	clGetPlatformIDs(platformsCount, platform, NULL);

	for (unsigned int i = 0; i < platformsCount; i++)
	{
		char pName[1024];
		cl_uint devicesCount;
		cl_device_id *device;

		clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, 1024, pName, NULL);
		cout << "Platform: " << pName << "\n";

		clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, 0, NULL, &devicesCount);
		device = (cl_device_id *)malloc(devicesCount * sizeof(cl_device_id));
		clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, devicesCount, device, NULL);

		for (unsigned int j = 0; j < devicesCount; j++)
		{
			char dName[1024];
			clGetDeviceInfo(device[j], CL_DEVICE_NAME, 1024, dName, NULL);
			cout << "Device: " << dName << "\n";
		}

		if (devicesCount > 0)
			gpuDevice = device[0];
		else {
			cout << "Nenalezeno GPU device\n";
			cin.ignore();
			free(device);
			return -1;
		}

		free(device);
	}

	free(platform);

	/* Create input matrix with random data */
	long double *matrix = new long double[MATRIX_SIZE*MATRIX_SIZE];
	for (int y = 0; y < MATRIX_SIZE; y++)
	{
		for (int x = 0; x < MATRIX_SIZE; x++)
		{
			matrix[y*MATRIX_SIZE + x] = (rand() % 10) - 5;
		}
	}

#ifdef DEBUG
	cout << setiosflags(ios::fixed) << setprecision(COUT_NUMBER_PRECISION) << endl;
	cout << "Input matrix:" << endl;

	for (int x = 0; x < MATRIX_SIZE; x++)
	{
		for (int y = 0; y < MATRIX_SIZE; y++)
		{
			cout << setw(COUT_NUMBER_WIDTH) << matrix[x*MATRIX_SIZE + y] << " ";
		}
		cout << endl;
	}
	cout << endl;
#endif

	cpu_determinant(matrix, MATRIX_SIZE);

#ifdef DEBUG
	cout << "Output matrix:" << endl;
	for (int x = 0; x < MATRIX_SIZE; x++)
	{
		for (int y = 0; y < MATRIX_SIZE; y++)
		{
			cout << setw(COUT_NUMBER_WIDTH) << matrix[x*MATRIX_SIZE + y] << " ";
		}
		cout << endl;
	}
	cout << endl;
#endif

	delete[] matrix;

	cin.ignore();

	return 0;
}