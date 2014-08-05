
#ifndef ALGO_CONTOUR_BASE_CNN_HPP
#define ALGO_CONTOUR_BASE_CNN_HPP

#include "contour/all.hpp"

template <typename t_neuro>
class c_cnn
{
	vector<t_neuro> neuro;

	public:

		c_cnn(const unsigned height, const unsigned width)
		{
			unsigned v, u;

			for(v = 0; v < height; v++)
				for(u = 0; u < width; u++)
					neuro.push_back(t_neuro(v, u));
		};

		Mat operator()(const Mat & Y, const Mat & U)
		{
			Mat n_Y = Mat::zeros(Y.size(), CV_64F);

			for(auto & it : neuro)
				n_Y.at<double>(it.i, it.j) = it(Y, U);

			return n_Y;
		}
};

class c_neuro
{
	protected:

		double x;
		unsigned t;

	public:

		const unsigned i, j;

		c_neuro(const unsigned t_i, const unsigned t_j);

		double operator()(const Mat & Y, const Mat & U);

		virtual double A(const Mat & Y, const int ci, const int cj) = 0;
		virtual double B(const Mat & U, const int ci, const int cj) = 0;
		virtual double I() = 0;
		virtual double f() = 0;
		virtual unsigned size() = 0;
};

class c_simple_neuro : public c_neuro
{
	public:

		c_simple_neuro(const unsigned t_i, const unsigned t_j) : c_neuro(t_i, t_j) { ; } ;

		double A(const Mat & Y, const int ci, const int cj);
		double B(const Mat & U, const int ci, const int cj);
		double I();
		double f();
		unsigned size();
};

class c_my_neuro : public c_neuro
{
	public:

		c_my_neuro(const unsigned t_i, const unsigned t_j) : c_neuro(t_i, t_j) { ; } ;

		double A(const Mat & Y, const int ci, const int cj);
		double B(const Mat & U, const int ci, const int cj);
		double I();
		double f();
		unsigned size();
};

#endif

