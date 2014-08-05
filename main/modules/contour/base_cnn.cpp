
#include "contour/base_cnn.hpp"

c_neuro::c_neuro(const unsigned t_i, const unsigned t_j) :
	i(t_i), j(t_j)
{
	t = 0;
	x = 0;
}

double c_neuro::operator()(const Mat & Y, const Mat & U)
{
	const int size_2 = size() / 2;
	int ci, cj, ti, tj;
	double sum = 0;

	for(ci = - size_2; ci <= size_2; ci ++)
		for(cj = - size_2; cj <= size_2; cj ++)
		{
			ti = i + ci;
			tj = j + cj;

			if(ti >= 0 && ti < Y.rows && tj >= 0 && tj < Y.cols)
				sum += A(Y, ci, cj) + B(U, ci, cj);
		}

	x = -x + sum + I();
	t ++;

	return f();
}

