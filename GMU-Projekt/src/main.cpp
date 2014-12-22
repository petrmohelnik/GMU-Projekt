#include "CL\opencl.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

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
	
	cin.ignore();

	return 0;
}