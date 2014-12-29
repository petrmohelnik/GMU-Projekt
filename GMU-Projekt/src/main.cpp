#include "CL\opencl.h"
#include "matrix_operations.h"
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>

//#define PRINT_MATRIX	// comment out for large matrix (> 16x16)

using namespace std;

char* loadFile(char *fname)
{
	char *buffer;

	ifstream file(fname, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		file.seekg(0, file.end);
		int length = (int)file.tellg();
		file.seekg(0, file.beg);

		buffer = new char[length + 1];

		file.read(buffer, length);
		file.close();

		buffer[length] = '\0';

		cout << "file " << fname << " loaded" << endl;
	}
	else
	{
		cout << "Unable to open file " << fname << endl;
		cin.ignore();
		exit(-1);
	}

	return buffer;
}

//http://stackoverflow.com/questions/24326432/convenient-way-to-show-opencl-error-codes
const char *getErrorString(cl_int error)
{
	switch (error){
		// run-time and JIT compiler errors
	case 0: return "CL_SUCCESS";
	case -1: return "CL_DEVICE_NOT_FOUND";
	case -2: return "CL_DEVICE_NOT_AVAILABLE";
	case -3: return "CL_COMPILER_NOT_AVAILABLE";
	case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case -5: return "CL_OUT_OF_RESOURCES";
	case -6: return "CL_OUT_OF_HOST_MEMORY";
	case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case -8: return "CL_MEM_COPY_OVERLAP";
	case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case -11: return "CL_BUILD_PROGRAM_FAILURE";
	case -12: return "CL_MAP_FAILURE";
	case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	case -16: return "CL_LINKER_NOT_AVAILABLE";
	case -17: return "CL_LINK_PROGRAM_FAILURE";
	case -18: return "CL_DEVICE_PARTITION_FAILED";
	case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

		// compile-time errors
	case -30: return "CL_INVALID_VALUE";
	case -31: return "CL_INVALID_DEVICE_TYPE";
	case -32: return "CL_INVALID_PLATFORM";
	case -33: return "CL_INVALID_DEVICE";
	case -34: return "CL_INVALID_CONTEXT";
	case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	case -36: return "CL_INVALID_COMMAND_QUEUE";
	case -37: return "CL_INVALID_HOST_PTR";
	case -38: return "CL_INVALID_MEM_OBJECT";
	case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case -40: return "CL_INVALID_IMAGE_SIZE";
	case -41: return "CL_INVALID_SAMPLER";
	case -42: return "CL_INVALID_BINARY";
	case -43: return "CL_INVALID_BUILD_OPTIONS";
	case -44: return "CL_INVALID_PROGRAM";
	case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	case -46: return "CL_INVALID_KERNEL_NAME";
	case -47: return "CL_INVALID_KERNEL_DEFINITION";
	case -48: return "CL_INVALID_KERNEL";
	case -49: return "CL_INVALID_ARG_INDEX";
	case -50: return "CL_INVALID_ARG_VALUE";
	case -51: return "CL_INVALID_ARG_SIZE";
	case -52: return "CL_INVALID_KERNEL_ARGS";
	case -53: return "CL_INVALID_WORK_DIMENSION";
	case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	case -56: return "CL_INVALID_GLOBAL_OFFSET";
	case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	case -58: return "CL_INVALID_EVENT";
	case -59: return "CL_INVALID_OPERATION";
	case -60: return "CL_INVALID_GL_OBJECT";
	case -61: return "CL_INVALID_BUFFER_SIZE";
	case -62: return "CL_INVALID_MIP_LEVEL";
	case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	case -64: return "CL_INVALID_PROPERTY";
	case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	case -66: return "CL_INVALID_COMPILER_OPTIONS";
	case -67: return "CL_INVALID_LINKER_OPTIONS";
	case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

		// extension errors
	case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	default: return "Unknown OpenCL error";
	}
}

void CheckOpenCLError(cl_int err, char *info)
{
	if (err != CL_SUCCESS) {
		cout << "ERROR: " << info << getErrorString(err) << "\n";
		cin.ignore();
		exit(-1);
	}
}

//z cviceni GMU
unsigned int iCeilTo(unsigned int data, unsigned int align_size)
{
	return ((data - 1 + align_size) / align_size) * align_size;
}

void printMatrix(float *matrix, int matrixWidth, float *result = NULL, int resultSize = 0)
{
	const int COUT_NUMBER_WIDTH = 7;
	const int COUT_NUMBER_PRECISION = 3;

	cout << setiosflags(ios::fixed) << setprecision(COUT_NUMBER_PRECISION) << endl;

#ifdef PRINT_MATRIX

	for (int y = 0; y < matrixWidth; y++)
	{
		for (int x = 0; x < matrixWidth; x++)
		{
			cout << setw(COUT_NUMBER_WIDTH) << matrix[y*matrixWidth + x] << " ";
		}
		if (resultSize != 0 && result != NULL)
		{
			cout << " | ";
			for (int x = 0; x < resultSize / matrixWidth; x++)
			{
				cout << setw(COUT_NUMBER_WIDTH) << result[y*(resultSize / matrixWidth) + x];
			}
		}

		cout << '\n';
	}
	cout << endl;
#endif
}

//ze cvik
double getEventTime(cl_event i_event)
{
	cl_ulong time_from, time_to;
	clGetEventProfilingInfo(i_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_from, NULL);
	clGetEventProfilingInfo(i_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_to, NULL);
	return double(time_to - time_from) / 1000000;
}

int main(int argc, char* argv[])
{
	cl_uint platformsCount;
	cl_platform_id *platform;
	cl_device_id gpuDevice;
	cl_int errMsg;
	cl_int matrixWidth = 1500;
	cl_int matrixSize = matrixWidth * matrixWidth;
	cl_float *matrix = (cl_float *)malloc(matrixSize * sizeof(cl_float));
	if (matrix == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	cl_float *gemColumn = (cl_float *)malloc(matrixWidth * sizeof(cl_float));
	if (gemColumn == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}

	srand(1);
	for (int i = 0; i < matrixSize; i++)
	{
		matrix[i] = (rand() / static_cast <float> (RAND_MAX)) * 10 - 5;
	}
	for (int i = 0; i < matrixWidth; i++)
	{
		gemColumn[i] = 1.0;
	}

	CheckOpenCLError(clGetPlatformIDs(0, NULL, &platformsCount), "clGetPlatformIDs: ");
	platform = (cl_platform_id *)malloc(platformsCount * sizeof(cl_platform_id));
	if (platform == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	CheckOpenCLError(clGetPlatformIDs(platformsCount, platform, NULL), "clGetPlatformIDs: ");

	for (unsigned int i = 0; i < platformsCount; i++)
	{
		char pName[1024];
		cl_uint devicesCount;
		cl_device_id *device;

		CheckOpenCLError(clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, 1024, pName, NULL), "clGetPlatformInfo: ");
		cout << "Platform: " << pName << "\n";

		CheckOpenCLError(clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, 0, NULL, &devicesCount), "clGetDeviceIDs: ");
		device = (cl_device_id *)malloc(devicesCount * sizeof(cl_device_id));
		if (device == NULL) {
			cout << "ERROR: malloc\n"; cin.ignore(); return -1;
		}
		CheckOpenCLError(clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, devicesCount, device, NULL), "clGetDeviceIDs: ");

		for (unsigned int j = 0; j < devicesCount; j++)
		{
			char dName[1024];
			CheckOpenCLError(clGetDeviceInfo(device[j], CL_DEVICE_NAME, 1024, dName, NULL), "clGetDeviceInfo: ");
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

	cl_context context = clCreateContext(NULL, 1, &gpuDevice, NULL, NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateContext: ");

	cl_command_queue queue = clCreateCommandQueue(context, gpuDevice, CL_QUEUE_PROFILING_ENABLE, &errMsg);
	CheckOpenCLError(errMsg, "clCreateContext: ");

	char *programSource = loadFile("src/matrix_operations.cl");
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&programSource, NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateProgramWithSource: ");
	if ((errMsg = clBuildProgram(program, 1, &gpuDevice, "-cl-nv-verbose", NULL, NULL)) == CL_BUILD_PROGRAM_FAILURE)
	{
		size_t logSize;
		CheckOpenCLError(clGetProgramBuildInfo(program, gpuDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize), "clGetProgramBuildInfo: ");
		char *log = (char *)malloc(logSize);
		CheckOpenCLError(clGetProgramBuildInfo(program, gpuDevice, CL_PROGRAM_BUILD_LOG, logSize, (void *)log, NULL), "clGetProgramBuildInfo: ");
		cout << log;
		cin.ignore();
	}
	size_t logSize;
	CheckOpenCLError(clGetProgramBuildInfo(program, gpuDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize), "clGetProgramBuildInfo: ");
	char *log = (char *)malloc(logSize);
	CheckOpenCLError(clGetProgramBuildInfo(program, gpuDevice, CL_PROGRAM_BUILD_LOG, logSize, (void *)log, NULL), "clGetProgramBuildInfo: ");
	cout << log;

	cl_kernel kernelDet = clCreateKernel(program, "determinant", &errMsg);
	CheckOpenCLError(errMsg, "clCreateKernel: ");
	cl_kernel kernelGem1 = clCreateKernel(program, "gem1", &errMsg);
	CheckOpenCLError(errMsg, "clCreateKernel: ");
	cl_kernel kernelGem2 = clCreateKernel(program, "gem2", &errMsg);
	CheckOpenCLError(errMsg, "clCreateKernel: ");
	cl_kernel kernelInverse = clCreateKernel(program, "inverse", &errMsg);
	CheckOpenCLError(errMsg, "clCreateKernel: ");

	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSize * sizeof(cl_float), NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem gemInputBufferMatrix = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixSize * sizeof(cl_float), NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem gemInputBufferColumn = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixWidth * sizeof(cl_float), NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem resultBufferDet = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixSize * sizeof(cl_float), NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem resultBufferGem = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixSize * sizeof(cl_float), NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem resultBufferInverse = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixSize * sizeof(cl_float), NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");

	cl_event inputEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event gemInputEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event gemInputColumnEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event kernelDetEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event resultDetEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event kernelGem1Event = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event kernelGem2Event = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event resultGemMatrixEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event resultGemColumnEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event kernelInverseEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event resultInverseEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");

	size_t local[2] = { 128, 128 };
	size_t maxWorkGroupSize, maxWorkItemSizes[10];
	clGetDeviceInfo(gpuDevice, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
	clGetDeviceInfo(gpuDevice, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 10, maxWorkItemSizes, NULL);
	while ((local[0] * local[1] > maxWorkGroupSize) || (local[0] > maxWorkItemSizes[0]) || (local[1] > maxWorkItemSizes[1])) {
		local[0] /= 2;
		local[1] /= 2;
	}

	size_t global[2];
	global[0] = (size_t)matrixWidth;
	global[1] = (size_t)matrixWidth;
	size_t local2 = local[0] * local[1];
	size_t global2 = (size_t)matrixWidth;

	cl_float resultDet;
	cl_float *resultGem = (cl_float *)malloc(matrixSize * sizeof(cl_float));
	if (resultGem == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	cl_float *resultGemColumn = (cl_float *)malloc(matrixWidth * sizeof(cl_float));
	if (resultGem == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	cl_float *resultInverse = (cl_float *)malloc(matrixSize * sizeof(cl_float));
	if (resultInverse == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}

	CheckOpenCLError(clSetKernelArg(kernelDet, 0, sizeof(cl_mem), &inputBuffer), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelDet, 1, sizeof(cl_mem), &resultBufferDet), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelDet, 2, sizeof(cl_int), &matrixWidth), "clSetKernelArg: ");

	CheckOpenCLError(clSetKernelArg(kernelInverse, 0, sizeof(cl_mem), &inputBuffer), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelInverse, 1, sizeof(cl_mem), &resultBufferInverse), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelInverse, 2, sizeof(cl_int), &matrixWidth), "clSetKernelArg: ");

	CheckOpenCLError(clEnqueueWriteBuffer(queue, inputBuffer, CL_FALSE, 0, matrixSize * sizeof(cl_float), matrix, 0, NULL, &inputEvent), "clEnqueueWriteBuffer: ");
	CheckOpenCLError(clEnqueueWriteBuffer(queue, gemInputBufferMatrix, CL_FALSE, 0, matrixSize * sizeof(cl_float), matrix, 0, NULL, &gemInputEvent), "clEnqueueWriteBuffer: 1: ");
	CheckOpenCLError(clEnqueueWriteBuffer(queue, gemInputBufferColumn, CL_FALSE, 0, matrixWidth * sizeof(cl_float), gemColumn, 0, NULL, &gemInputColumnEvent), "clEnqueueWriteBuffer: 1: ");

	//CheckOpenCLError(clEnqueueWriteBuffer(queue, resultBufferDet, CL_FALSE, 0, 1, 0, 0, NULL, NULL), "clEnqueueWriteBuffer: ");
	CheckOpenCLError(clEnqueueNDRangeKernel(queue, kernelDet, 2, NULL, global, NULL, 0, NULL, &kernelDetEvent), "clEnqueueNDRangeKernel: 1: ");
	CheckOpenCLError(clEnqueueReadBuffer(queue, resultBufferDet, CL_FALSE, 0, sizeof(cl_float), &resultDet, 0, NULL, &resultDetEvent), "clEnqueueWriteBuffer: ");

	float gem1Time = 0.0, gem2Time = 0.0;
	for (int i = 0; i < (matrixWidth - 1); i++)
	{
		CheckOpenCLError(clSetKernelArg(kernelGem1, 0, sizeof(cl_mem), &gemInputBufferMatrix), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem1, 1, sizeof(cl_mem), &gemInputBufferColumn), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem1, 2, sizeof(cl_mem), &resultBufferGem), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem1, 3, sizeof(cl_int), &matrixWidth), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem1, 4, sizeof(cl_int), &i), "clSetKernelArg: ");

		CheckOpenCLError(clEnqueueNDRangeKernel(queue, kernelGem1, 1, NULL, &global2, NULL, 0, NULL, &kernelGem1Event), "clEnqueueNDRangeKernel: 2: ");

		CheckOpenCLError(clSetKernelArg(kernelGem2, 0, sizeof(cl_mem), &gemInputBufferMatrix), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem2, 1, sizeof(cl_mem), &gemInputBufferColumn), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem2, 2, sizeof(cl_mem), &resultBufferGem), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem2, 3, sizeof(cl_int), &matrixWidth), "clSetKernelArg: ");
		CheckOpenCLError(clSetKernelArg(kernelGem2, 4, sizeof(cl_int), &i), "clSetKernelArg: ");

		CheckOpenCLError(clEnqueueNDRangeKernel(queue, kernelGem2, 2, NULL, global, NULL, 0, NULL, &kernelGem2Event), "clEnqueueNDRangeKernel: 2: ");

		CheckOpenCLError(clFinish(queue), "clFinish: ");
		gem1Time += getEventTime(kernelGem1Event);
		gem2Time += getEventTime(kernelGem2Event);
		clReleaseEvent(kernelGem1Event);
		clReleaseEvent(kernelGem2Event);
	}

	CheckOpenCLError(clEnqueueReadBuffer(queue, gemInputBufferMatrix, CL_FALSE, 0, matrixSize * sizeof(cl_float), resultGem, 0, NULL, &resultGemMatrixEvent), "clEnqueueWriteBuffer: ");
	CheckOpenCLError(clEnqueueReadBuffer(queue, gemInputBufferColumn, CL_FALSE, 0, matrixWidth * sizeof(cl_float), resultGemColumn, 0, NULL, &resultGemColumnEvent), "clEnqueueWriteBuffer: ");

	//CheckOpenCLError(clEnqueueWriteBuffer(queue, resultBufferInverse, CL_FALSE, 0, matrixSize, resultInit, 0, NULL, NULL), "clEnqueueWriteBuffer: ");
	CheckOpenCLError(clEnqueueNDRangeKernel(queue, kernelInverse, 2, NULL, global, NULL, 0, NULL, &kernelInverseEvent), "clEnqueueNDRangeKernel: 3: ");

	CheckOpenCLError(clEnqueueReadBuffer(queue, resultBufferInverse, CL_FALSE, 0, matrixSize * sizeof(cl_float), resultInverse, 0, NULL, &resultInverseEvent), "clEnqueueWriteBuffer: ");

	CheckOpenCLError(clFinish(queue), "clFinish: ");

	float *detMatrix = (float *)malloc(matrixSize * sizeof(float));
	if (detMatrix == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	memcpy(detMatrix, matrix, matrixSize * sizeof(float));

	float *gemMatrix = (float *)malloc(matrixSize * sizeof(float));
	if (gemMatrix == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	memcpy(gemMatrix, matrix, matrixSize * sizeof(float));

	float *invMatrix = (float *)malloc(matrixSize * sizeof(float));
	if (invMatrix == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	memcpy(invMatrix, matrix, matrixSize * sizeof(float));

	float *invResult = (float *)malloc(matrixSize * sizeof(float));
	if (invResult == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}
	float *gemResult = (float *)malloc(matrixWidth * sizeof(float));
	if (gemResult == NULL) {
		cout << "ERROR: malloc\n"; cin.ignore(); return -1;
	}

	cout << "Input matrix (" << matrixWidth << "x" << matrixWidth << ")" << endl;
	printMatrix(detMatrix, matrixWidth);

	//tohle je cpu cast z toho navodu na cuda
	////////////////////
	clock_t TimeGemCPU = clock();
	float *resultGemColumnGPU = (float*)malloc(sizeof(float)*(matrixWidth));
	int i, j;
	for (i = 0; i < matrixWidth; i++){
		resultGemColumnGPU[matrixWidth - i - 1] = resultGemColumn[matrixWidth - i - 1];
		for (j = 0; j < i; j++)
		{
			resultGemColumnGPU[matrixWidth - i - 1] -= *(resultGem + matrixWidth*(matrixWidth - i - 1) + (matrixWidth - j - 1)) * resultGemColumnGPU[matrixWidth - j - 1];
		}
		resultGemColumnGPU[matrixWidth - i - 1] = resultGemColumnGPU[matrixWidth - i - 1] / *(resultGem + matrixWidth*(matrixWidth - i - 1) + (matrixWidth - i - 1));
	}
	TimeGemCPU = clock() - TimeGemCPU;

	//int j;
	/*for (int i = matrixWidth - 1; i >= 0; i--)
	{
	float sum = 0.0;

	for (j = matrixWidth - 1; j>i; j--)
	{
	sum = sum + result[j] * resultGem[i*(matrixWidth + 1) + j];
	}
	float rvalue = resultGem[i*(matrixWidth + 1) + matrixWidth] - sum;
	result[i] = rvalue / resultGem[i *(matrixWidth + 1) + j];
	}
	*/
	//Displaying the result
	/*printf("\n\t\tVALUES OF VARIABLES\n\n");
	for (int i = 0; i<matrixWidth; i++)
	{
	printf("[X%d] = %+f\n", i, result[i]);
	}*/
	////////////////////

	cout << "write: " << (getEventTime(gemInputEvent) + getEventTime(gemInputColumnEvent)) << "ms\n";
	cout << "kernel1: " << gem1Time << "ms\n";
	cout << "kernel2: " << gem2Time << "ms\n";
	cout << "TimeGemCPU: " << float(TimeGemCPU) << "ms\n";
	cout << "read: " << (getEventTime(resultGemMatrixEvent) + getEventTime(resultGemColumnEvent)) << "ms\n";

	float d;
	int exp;

	clock_t cpuDetTime = clock();
	determinant(detMatrix, &d, &exp, matrixWidth);
	cpuDetTime = clock() - cpuDetTime;

	cout << "Determinant CPU output matrix:" << endl;
	printMatrix(detMatrix, matrixWidth);

	cout << "CPU Determinant: " << d;
	if (exp > 0)
	{
		cout << "e+" << exp;
	}
	cout << endl;

	clock_t cpuGemTime = clock();
	gem(gemMatrix, gemResult, matrixWidth);
	cpuGemTime = clock() - cpuGemTime;

	cout << "\n\nGEM CPU output matrix (" << matrixWidth << "x" << matrixWidth << ")" << endl;
	printMatrix(gemMatrix, matrixWidth, gemResult, matrixWidth);

	cout << "\n\nGEM GPU output matrix (" << matrixWidth << "x" << matrixWidth << ")" << endl;
	printMatrix(resultGem, matrixWidth, resultGemColumnGPU, matrixWidth);

	clock_t cpuInvTime = clock();
	inverse(invMatrix, invResult, matrixWidth);
	cpuInvTime = clock() - cpuInvTime;

	cout << endl << "Inverted CPU output matrix: " << endl;
	printMatrix(gemMatrix, matrixWidth, invResult, matrixSize);

	cout << "CPU/GPU Times:\nCPU determinant (ms): " << cpuDetTime << "\nGPU determinant (ms): " << "\nCPU GEM (ms): "
		<< cpuGemTime << "\nGPU GEM (ms): " << gem2Time << "\nCPU Invert (ms): " << cpuInvTime << "\nGPU Invert (ms): " << endl;

	CheckOpenCLError(clReleaseMemObject(inputBuffer), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(gemInputBufferMatrix), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(gemInputBufferColumn), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(resultBufferDet), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(resultBufferGem), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(resultBufferInverse), "clReleaseMemObject: ");

	CheckOpenCLError(clReleaseEvent(inputEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(kernelDetEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(resultDetEvent), "clReleaseEvent: ");
	//	CheckOpenCLError(clReleaseEvent(kernelGem1Event), "clReleaseEvent: ");
	//	CheckOpenCLError(clReleaseEvent(kernelGem2Event), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(resultGemMatrixEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(resultGemColumnEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(kernelInverseEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(resultInverseEvent), "clReleaseEvent: ");

	free(resultGem);
	free(resultInverse);

	CheckOpenCLError(clReleaseKernel(kernelDet), "clReleaseKernel: ");
	CheckOpenCLError(clReleaseKernel(kernelGem1), "clReleaseKernel: ");
	CheckOpenCLError(clReleaseKernel(kernelGem2), "clReleaseKernel: ");
	CheckOpenCLError(clReleaseKernel(kernelInverse), "clReleaseKernel: ");

	CheckOpenCLError(clReleaseProgram(program), "clReleaseProgram: ");
	CheckOpenCLError(clReleaseContext(context), "clReleaseContext: ");

	free(gemResult);
	free(invResult);
	free(invMatrix);
	free(gemMatrix);
	free(detMatrix);
	free(matrix);

	cin.ignore();

	return 0;
}