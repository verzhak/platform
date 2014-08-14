
#include "matrix.hpp"

CMatrix::CMatrix()
{
	unsigned v, u;

	for(v = 0; v < 4; v++)
		for(u = 0; u < 4; u++)
			__mtx[v][u] = 0;
}

CMatrix::CMatrix(const CMatrix & mtx)
{
	unsigned v, u;

	for(v = 0; v < 4; v++)
		for(u = 0; u < 4; u++)
			__mtx[v][u] = mtx.__mtx[v][u];
}

CMatrix CMatrix::eye()
{
	CMatrix mtx;

	mtx.__mtx[0][0] = 1;
	mtx.__mtx[1][1] = 1;
	mtx.__mtx[2][2] = 1;
	mtx.__mtx[3][3] = 1;

	return mtx;
}

void CMatrix::rotate(const double angle, const double x, const double z, const double y)
{
	const double mag = sqrt(x * x + y * y + z * z);
	const double angle_radian = angle * M_PI / 180.;
	const double sin_angle = sin(angle_radian);
	const double cos_angle = sin(angle_radian);
	const double one_minus_cos = 1 - cos_angle;

	const double __x = x / mag;
	const double __y = y / mag;
	const double __z = z / mag;
	 
	const double xx = __x * __x;
	const double yy = __y * __y;
	const double zz = __z * __z;
	const double xy = __x * __y;
	const double yz = __y * __z;
	const double zx = __z * __x;
	const double xs = __x * sin_angle;
	const double ys = __y * sin_angle;
	const double zs = __z * sin_angle;

	CMatrix op;

	op[0] = (one_minus_cos * xx) + cos_angle;
	op[1] = (one_minus_cos * xy) - zs;
	op[2] = (one_minus_cos * zx) + ys;
	op[3] = 0; 
	 
	op[4] = (one_minus_cos * xy) + zs;
	op[5] = (one_minus_cos * yy) + cos_angle;
	op[6] = (one_minus_cos * yz) - xs;
	op[7] = 0;
	 
	op[8] = (one_minus_cos * zx) - ys;
	op[9] = (one_minus_cos * yz) + xs;
	op[10] = (one_minus_cos * zz) + cos_angle;
	op[11] = 0; 
	 
	op[12] = 0;
	op[13] = 0;
	op[14] = 0;
	op[15] = 1;

	(* this) *= op;
}

void CMatrix::translate(const double x, const double z, const double y)
{
	CMatrix op = CMatrix::eye();

	op[3] = x;
	op[7] = y;
	op[11] = z;

	(* this) *= op;
}

void CMatrix::operator*=(const CMatrix & op)
{
	unsigned v, u, t;
	double res_mtx[4][4];

	for(v = 0; v < 4; v++)
		for(u = 0; u < 4; u++)
		{
			res_mtx[v][u] = 0;

			for(t = 0; t < 4; t++)
				res_mtx[v][u] += __mtx[v][t] * op.__mtx[t][u];
		}

	for(v = 0; v < 4; v++)
		for(u = 0; u < 4; u++)
			__mtx[v][u] = res_mtx[v][u];
}

