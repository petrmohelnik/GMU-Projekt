#include "CL\opencl.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

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

int main(int argc, char* argv[])
{
	cl_uint platformsCount;
	cl_platform_id *platform;
	cl_device_id gpuDevice;
	cl_int errMsg;
	cl_int matrixWidth = 64;
	cl_int matrixSize = matrixWidth * matrixWidth;
	cl_double *matrix = (cl_double *) malloc(matrixSize * sizeof(cl_double));

	CheckOpenCLError(clGetPlatformIDs(0, NULL, &platformsCount), "clGetPlatformIDs: ");
	platform = (cl_platform_id *)malloc(platformsCount * sizeof(cl_platform_id));
	if (platform == NULL) {
		cout << "ERROR: malloc\n";
		cin.ignore();
		return -1;
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
			cout << "ERROR: malloc\n";
			cin.ignore();
			return -1;
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
	if ((errMsg = clBuildProgram(program, 1, &gpuDevice, "", NULL, NULL)) == CL_BUILD_PROGRAM_FAILURE)
	{
		size_t logSize;
		CheckOpenCLError(clGetProgramBuildInfo(program, gpuDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize), "clGetProgramBuildInfo: ");
		char *log = (char *)malloc(logSize);
		CheckOpenCLError(clGetProgramBuildInfo(program, gpuDevice, CL_PROGRAM_BUILD_LOG, logSize, (void *)log, NULL), "clGetProgramBuildInfo: ");
		cout << log;
		cin.ignore();
	}

	cl_kernel kernelDet = clCreateKernel(program, "determinant", &errMsg);
	CheckOpenCLError(errMsg, "clCreateKernel: ");
	cl_kernel kernelGem = clCreateKernel(program, "gem", &errMsg);
	CheckOpenCLError(errMsg, "clCreateKernel: ");
	cl_kernel kernelInverse = clCreateKernel(program, "inverse", &errMsg);
	CheckOpenCLError(errMsg, "clCreateKernel: ");

	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSize, NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem resultBufferDet = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixSize, NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem resultBufferGem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixSize, NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");
	cl_mem resultBufferInverse = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixSize, NULL, &errMsg);
	CheckOpenCLError(errMsg, "clCreateBuffer: ");

	cl_event inputEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event kernelDetEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event resultDetEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event kernelGemEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event resultGemEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event kernelInverseEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");
	cl_event resultInverseEvent = clCreateUserEvent(context, &errMsg);
	CheckOpenCLError(errMsg, "clCreateUserEvent: ");

	size_t local[2] = { 32, 32 };
	size_t global[2];
	global[0] = iCeilTo(matrixWidth, local[0]);
	global[1] = global[0];
	cl_double resultDet;
	cl_double *resultGem = (cl_double *) malloc(matrixSize * sizeof(cl_double));
	if (resultGem == NULL) {
		cout << "ERROR: malloc\n";
		cin.ignore();
		return -1;
	}
	cl_double *resultInverse = (cl_double *)malloc(matrixSize * sizeof(cl_double));
	if (resultInverse == NULL) {
		cout << "ERROR: malloc\n";
		cin.ignore();
		return -1;
	}

	CheckOpenCLError(clSetKernelArg(kernelDet, 0, sizeof(cl_mem), &inputBuffer), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelDet, 1, sizeof(cl_mem), &resultBufferDet), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelDet, 2, sizeof(cl_int), &matrixSize), "clSetKernelArg: ");

	CheckOpenCLError(clSetKernelArg(kernelGem, 0, sizeof(cl_mem), &inputBuffer), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelGem, 1, sizeof(cl_mem), &resultBufferGem), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelGem, 2, sizeof(cl_int), &matrixSize), "clSetKernelArg: ");

	CheckOpenCLError(clSetKernelArg(kernelInverse, 0, sizeof(cl_mem), &inputBuffer), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelInverse, 1, sizeof(cl_mem), &resultBufferInverse), "clSetKernelArg: ");
	CheckOpenCLError(clSetKernelArg(kernelInverse, 2, sizeof(cl_int), &matrixSize), "clSetKernelArg: ");

	CheckOpenCLError(clEnqueueWriteBuffer(queue, inputBuffer, CL_FALSE, 0, matrixSize, matrix, 0, NULL, &inputEvent), "clEnqueueWriteBuffer: ");
	
	//CheckOpenCLError(clEnqueueWriteBuffer(queue, resultBufferDet, CL_FALSE, 0, 1, 0, 0, NULL, NULL), "clEnqueueWriteBuffer: ");
	CheckOpenCLError(clEnqueueNDRangeKernel(queue, kernelDet, 2, NULL, global, local, 0, NULL, &kernelDetEvent), "clEnqueueNDRangeKernel: ");
	CheckOpenCLError(clEnqueueReadBuffer(queue, resultBufferDet, CL_FALSE, 0, 1, &resultDet, 0, NULL, &resultDetEvent), "clEnqueueWriteBuffer: ");

	//CheckOpenCLError(clEnqueueWriteBuffer(queue, resultBufferGem, CL_FALSE, 0, matrixSize, resultInit, 0, NULL, NULL), "clEnqueueWriteBuffer: ");
	CheckOpenCLError(clEnqueueNDRangeKernel(queue, kernelGem, 2, NULL, global, local, 0, NULL, &kernelGemEvent), "clEnqueueNDRangeKernel: ");
	CheckOpenCLError(clEnqueueReadBuffer(queue, resultBufferGem, CL_FALSE, 0, matrixSize, resultGem, 0, NULL, &resultGemEvent), "clEnqueueWriteBuffer: ");

	//CheckOpenCLError(clEnqueueWriteBuffer(queue, resultBufferInverse, CL_FALSE, 0, matrixSize, resultInit, 0, NULL, NULL), "clEnqueueWriteBuffer: ");
	CheckOpenCLError(clEnqueueNDRangeKernel(queue, kernelInverse, 2, NULL, global, local, 0, NULL, &kernelInverseEvent), "clEnqueueNDRangeKernel: ");
	CheckOpenCLError(clEnqueueReadBuffer(queue, resultBufferInverse, CL_FALSE, 0, matrixSize, resultInverse, 0, NULL, &resultInverseEvent), "clEnqueueWriteBuffer: ");
	
	CheckOpenCLError(clReleaseMemObject(inputBuffer), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(resultBufferDet), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(resultBufferGem), "clReleaseMemObject: ");
	CheckOpenCLError(clReleaseMemObject(resultBufferInverse), "clReleaseMemObject: ");

	CheckOpenCLError(clReleaseEvent(inputEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(kernelDetEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(resultDetEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(kernelGemEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(resultGemEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(kernelInverseEvent), "clReleaseEvent: ");
	CheckOpenCLError(clReleaseEvent(resultInverseEvent), "clReleaseEvent: ");

	free(resultGem);
	free(resultInverse);

	CheckOpenCLError(clReleaseKernel(kernelDet), "clReleaseKernel: ");
	CheckOpenCLError(clReleaseKernel(kernelGem), "clReleaseKernel: ");
	CheckOpenCLError(clReleaseKernel(kernelInverse), "clReleaseKernel: ");

	CheckOpenCLError(clReleaseProgram(program), "clReleaseProgram: ");
	CheckOpenCLError(clReleaseContext(context), "clReleaseContext: ");	
	
	cin.ignore();

	return 0;
}