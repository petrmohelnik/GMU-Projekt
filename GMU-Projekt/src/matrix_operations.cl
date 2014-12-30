#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void determinant(__global float *m, __global float *r, int s, int i)
{
	int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	if ((g_x + i < s - 1) && (g_y + i < s)) {
		m[(g_x + i + 1) * s + g_y + i] -= r[g_x + i + 1] * m[i * s + g_y + i];
	}
}

__kernel void gem1(__global float *m, __global float *c, __global float *r, int s, int i)
{
	int g = (int)get_global_id(0);

	if (g + i < s - 1) {
		r[g + i + 1] = native_divide(m[(g + i + 1) * s + i], m[i * s + i]);
	}
}

__kernel void gem2(__global float *m, __global float *c, __global float* r, int s, int i)
{
	int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	if ((g_x + i < s - 1) && (g_y + i < s)) {
		m[(g_x + i + 1) * s + g_y + i] -= r[g_x + i + 1] * m[i * s + g_y + i];

		if (g_y == 0) {
			c[g_x + i + 1] -= r[g_x + i + 1] * c[i];
		}
	}
}

__kernel void gem(__global float *m, __global float *c, int s, int i)
{
	/*int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	float divider = m[i * s + i];
	barrier(CLK_LOCAL_MEM_FENCE);

	m[i * s + g_x] = native_divide(m[i * s + g_x], divider);
	c[i] = native_divide(c[i], divider);

	barrier(CLK_LOCAL_MEM_FENCE);
	float rat = native_divide(m[g_y * s + i], m[i * s + i]);

	if ((g_x < s) && (g_y < s) && i != g_y)
	{
	m[g_y * s + g_x] -= rat * m[i * s + g_x];
	c[g_y] -= rat * c[i];
	}*/
}

__kernel void inverse(__global float *m, __global float *im, __global float* r, int s, int i)
{
	/*int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	float divider = m[i * s + i];
	barrier(CLK_GLOBAL_MEM_FENCE);

	m[i * s + g_x] = native_divide(m[i * s + g_x], divider);
	im[i * s + g_x] = native_divide(im[i * s + g_x], divider);

	barrier(CLK_GLOBAL_MEM_FENCE);
	float rat = native_divide(m[g_y * s + i], m[i * s + i]);

	if ((g_x < s) && (g_y < s) && i != g_y)
	{
	m[g_y * s + g_x] -= r[g_x + i + 1] * m[i * s + g_x];
	im[g_y * s + g_x] -= r[g_x + i + 1] * im[i * s + g_x];

	//im[(g_y + i + 1) * s + g_x + i] -= r[g_x + i + 1] * im[i * s + g_x];
	m[g_y * s + g_x] -= rat * m[i * s + g_x];
	im[g_y * s + g_x] -= rat * im[i * s + g_x];
	}*/

	/*int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	if ((g_x + i < s - 1) && (g_y + i < s)) {
	m[(g_x + i + 1) * s + g_y + i] -= r[g_x + i + 1] * m[i * s + g_y + i];

	if (g_y == 0) {
	im[(g_x + i + 1) * s + g_y + i] -= r[g_x + i + 1] * im[i * s + g_y + i];
	}
	}*/
}