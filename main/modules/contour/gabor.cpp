
#include "contour/contour.hpp"

// TODO

class c_gabor
{
	Mat kernel[2];

	public:

		const double sigma, lambda, omega, theta;

		c_gabor(const unsigned, const double, const double, const double, const double);
		void apply(const Mat &, double *);
		inline unsigned size();
		void print();
};

class c_gabor_set
{
	vector<c_gabor> filters;

	public:

		c_gabor_set(const unsigned, const unsigned, const double, const double, const double);
		void apply(const Mat &, vector< vector<Mat>> &, const bool, const Mat &);
		void apply(const Mat &, Mat &, Mat &, Mat &, const double[], const bool, const Mat &);
		Mat apply(const Mat &, const int, Mat &, Mat &, const double[]);
};

// ############################################################################ 

/*
 * sigma - среднеквадратичное отклонение распределения Гаусса
 * lambda - частота волны (гармоника) по x
 * omega - частота волны (гармоника) по y
 * theta - угол (orientation of the normal to the parallel stripes of a Gabor function)
 */
c_gabor::c_gabor(const unsigned size, const double t_sigma, const double t_lambda, const double t_omega, const double t_theta) :
	sigma(t_sigma), lambda(t_lambda), omega(t_omega), theta(t_theta)
{
	const int size_2 = size / 2;
	const double sigma_p2 = -2 * sigma * sigma;
	unsigned v, u;
	double x, y, xs, ys, t_exp, phi, sum_0, value_0, value_1;

	kernel[0].create(size, size, CV_64F);
	kernel[1].create(size, size, CV_64F);

	sum_0 = 0;

	for(y = - size_2, v = 0; v < size; y++, v++)
		for(x = - size_2, u = 0; u < size; x++, u++)
		{
			xs = x * cos(theta) + y * sin(theta);
			ys = - x * sin(theta) + y * cos(theta);
			t_exp = exp((xs * xs + ys * ys) / sigma_p2);
			phi = lambda * xs + omega * ys;

			value_0 = t_exp * cos(phi);		// Симметричное ядро
			value_1 = t_exp * sin(phi);		// Асимметричное ядро

			sum_0 += value_0;

			kernel[0].at<double>(v, u) = value_0;		// Симметричное ядро
			kernel[1].at<double>(v, u) = value_1;		// Асимметричное ядро
		}

	kernel[0] /= sum_0;
}

void c_gabor::print()
{
	const unsigned t_size = size();
	unsigned v, u;

	printf("\nФильтр size = %u; sigma = %lf; lambda = %lf; omega = %lf; theta = %lf (%lf)\n\nСимметричное ядро =\n[\n", t_size, sigma, lambda, omega, theta, theta * 180 / M_PI);

	for(v = 0; v < t_size; v++)
	{
		printf("\t");

		for(u = 0; u < t_size; u++)
			printf("%lf ", kernel[0].at<double>(v, u));

		printf("\n");
	}

	printf("]\n\nАсимметричное ядро =\n[\n");

	for(v = 0; v < t_size; v++)
	{
		printf("\t");

		for(u = 0; u < t_size; u++)
			printf("%lf ", kernel[1].at<double>(v, u));

		printf("\n");
	}

	printf("]\n");
}

void c_gabor::apply(const Mat & src, double * value)
{
	unsigned v, u, q;
	const unsigned t_size = size();

	for(q = 0; q < 2; q++)
	{
		value[q] = 0;

		for(v = 0; v < t_size; v++)
			for(u = 0; u < t_size; u++)
				value[q] += kernel[q].at<double>(v, u) * src.at<uint8_t>(v, u);
	}
}

unsigned c_gabor::size()
{
	return kernel[0].rows;
}

// ############################################################################

c_gabor_set::c_gabor_set(const unsigned num, const unsigned size, const double sigma, const double lambda, const double omega)
{
	const double step = 2 * M_PI / num;
	unsigned v;
	double theta;

	filters.clear();

	for(v = 0, theta = 0; v < num; v++, theta += step)
		filters.push_back(c_gabor(size, sigma, lambda, omega, theta));
}

void c_gabor_set::apply(const Mat & src, vector< vector<Mat>> & dst, const bool is_mask = false, const Mat & mask = Mat())
{
	const unsigned height = src.rows, width = src.cols, fnum = filters.size(), fsize = filters[0].size();
	const unsigned fsize_2 = fsize / 2;
	const unsigned to_v = height - fsize_2, to_u = width - fsize_2;
	unsigned t, v, u, q;
	double value[2];

	for(q = 0; q < 2; q++)
	{
		dst.push_back(vector<Mat>());

		for(t = 0; t < fnum; t++)
			dst[q].push_back(Mat::zeros(height, width, CV_64F));
	}

	for(v = fsize_2; v < to_v; v++)
		for(u = fsize_2; u < to_u; u++)
			if(! is_mask || (is_mask && mask.at<uint8_t>(v, u)))
				for(t = 0; t < fnum; t++)
				{
					filters[t].apply(src(Range(v - fsize_2, v + fsize_2), Range(u - fsize_2, u + fsize_2)), value);

					for(q = 0; q < 2; q++)
						dst[q][t].at<double>(v, u) = value[q];
				}
}

void c_gabor_set::apply(const Mat & src, Mat & dst, Mat & angle, Mat & angle_color, const double T[], const bool is_mask = false, const Mat & mask = Mat())
{
	const unsigned height = src.rows, width = src.cols, fnum = filters.size();
	unsigned value_u, v, u, q, t, pixel_u;
	int max_angle_ind;
	double value_d, sum_value_d, alpha, coef, max_value_d;
	vector< vector<Mat>> f_dst, f_dst_u8;
	Mat temp;

	dst = Mat::zeros(height, width, CV_8U);
	angle = Mat::zeros(height, width, CV_64F);
	angle_color = Mat::zeros(height, width, CV_8UC3);
	apply(src, f_dst, is_mask, mask);

	for(q = 0; q < 2; q++)
	{
		f_dst_u8.push_back(vector<Mat>());

		for(t = 0; t < fnum; t++)
			f_dst_u8[q].push_back(Mat(height, width, CV_8U));
	}

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
			if(! is_mask || (is_mask && mask.at<uint8_t>(v, u)))
			{
				for(q = 0, sum_value_d = 0; q < 2; q++)
					for(t = 0; t < fnum; t++)
					{
						uint8_t & pixel = f_dst_u8[q][t].at<uint8_t>(v, u);
						value_d = fabs(f_dst[q][t].at<double>(v, u));

						// TODO
						if(q == 1)
						{
							sum_value_d += value_d;

							if(! t || max_value_d < value_d)
							{
								max_value_d = value_d;
								max_angle_ind = t;
							}
						}
					}

				sum_value_d /= fnum;

				if(sum_value_d >= T[0])
				{
					dst.at<uint8_t>(v, u) = sum_value_d;
					angle.at<double>(v, u) = max_angle_ind * 2 * M_PI / fnum;
					angle_color.at<Vec3b>(v, u) = to_color<Vec3b>(max_angle_ind * (0xFFFFFF / fnum));
				}
				else
					angle.at<double>(v, u) = -1;
			}
	
	// ############################################################################ 

	for(q = 0; q < 2; q++)
		for(t = 0; t < fnum; t++)
		{
			char fname[4096];
			sprintf(fname, "%.2lf_%s.png", 180. / fnum * t, q ? "a" : "s");
			pyrUp(f_dst_u8[q][t], temp);
		}
}

Mat c_gabor_set::apply(const Mat & src, const int level, Mat & angle, Mat & angle_color, const double T[])
{
	Mat t_src, t_dst, t_angle, t_angle_color, dst;

	pyrDown(src, t_src);

	if(level == 0)
		apply(t_src, t_dst, angle, angle_color, T);
	else
		apply(t_src, t_dst, t_angle, t_angle_color, T, true, apply(t_src, level - 1, angle, angle_color, T));

	pyrUp(t_dst, dst);
	pyrUp(t_angle, angle);
	pyrUp(t_angle_color, angle_color);

	return dst;
}

// ############################################################################ 

Mat gabor_(const Mat & src, Mat & angle, Mat & angle_color)
{
	const double T[] = { 15, 20, 10 };
	const unsigned qnum = 20;	// Количество отсчетов квантования угла
	const unsigned fsize = 3;	// Размер фильтра
	const double sigma = 8.,	// Среднеквадратичное отклонение распределения Гаусса
		lambda = M_PI / 6,		// Частота волны (гармоника) по x
		omega = M_PI / 6;		// Частота волны (гармоника) по y

	// ############################################################################ 

	Mat src_gray, t_dst, dst;
	c_gabor_set gs(qnum, fsize, sigma, lambda, omega);

	cvtColor(src, src_gray, CV_RGB2GRAY);
	t_dst = gs.apply(src_gray, 1, angle, angle_color, T);
	threshold(t_dst, dst, T[2], 255, THRESH_BINARY);

	return dst;
}

// ############################################################################ 

Mat gabor(const Mat & src)
{
	Mat p_src, angle, angle_color, dst, t_dst;
	
//	p_src = preprocess(src);
	p_src = src;

	dst = gabor_(p_src, angle, angle_color);
	
//	dst = skeleton_(dst);
//	dst = approx_(dst);

	return dst;
}

