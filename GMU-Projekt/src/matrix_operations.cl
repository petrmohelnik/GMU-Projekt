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

__kernel void inv1(__global float *m, __global float *im, int s, int i)
{
	int g = (int)get_global_id(0);

	if(g < 2 * s) {
		if (g < s)
			m[i * s + g] /= m[i * s + i];
		else
			im[i * s + g - s] /= m[i * s + i];
	}
}

__kernel void inv2(__global float *m, __global float *im, int s, int i)
{
	int g_x = (int)get_global_id(0);
	int g_y = (int)get_global_id(1);

	if((g_x < s) && (g_y < 2 * s)) {
		if(g_x != i) {
			if(g_y < s)
				m[g_x * s + g_y] -= m[g_x * s + i] * m[i * s + g_y];
			else
				im[g_x * s + g_y - s] -= m[g_x * s + i] * im[i * s + g_y - s];
		}
	}
}