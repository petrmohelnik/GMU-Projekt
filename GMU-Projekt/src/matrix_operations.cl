#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void determinant(__global double *m, __global double *r, int s)
{
}

__kernel void gem(__global double *m, __global double *r, int s)
{
	int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	if((g_x < (s + 1)) && (g_y < s)) 
	{
		r[g_y * (s + 1) + g_x] = m[g_y * (s + 1) + g_x];
		barrier(CLK_GLOBAL_MEM_FENCE);
		for(int i = 1; i < s; i++)
		{
			if((g_y + i) < s)
			{
				float var = -1.0 * native_divide((float)r[(i - 1) * (s + 1) + (i - 1)], (float)r[(i + g_y) * (s + 1) + (i - 1)]);
				r[(i + g_y) * (s + 1) + g_x] = r[(i-1) * (s + 1) + g_x] + (var * r[(i + g_y) * (s + 1) + g_x]);
			}
			barrier(CLK_GLOBAL_MEM_FENCE);
		}
	}
}

__kernel void inverse(__global double *m, __global double *r, int s)
{
	int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	if((g_x < (s + 1)) && (g_y < s)) 
	{
		r[g_y * (s + 1) + g_x] = 1.0;
	}
}