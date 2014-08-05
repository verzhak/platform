
#include "contour/contour.hpp"

class c_noise
{

	public:

		virtual double operator()(const double, const double) = 0;

};

class c_gauss_noise : public c_noise
{

	const double sigma, sigma_2_2, pi_sigma_2_2;

	public:

		c_gauss_noise(const double);

		double operator()(const double, const double);
};

class c_deblurring
{

	public:

		virtual Mat operator()(const Mat &) = 0;

};

class c_deblurring_kernel : public c_deblurring
{
	protected:

		const unsigned kernel_height, kernel_width;
		Mat kernel;

	public:

		c_deblurring_kernel(c_noise *, const unsigned, const unsigned);

};

class c_wiener : public c_deblurring_kernel
{

	public:

		c_wiener(c_noise *, const unsigned, const unsigned, const unsigned, const unsigned, const double);

		Mat operator()(const Mat &);

};

class c_richardson_lucy : public c_deblurring_kernel
{

	const unsigned max_iter;
	Mat kernel_flip;

	public:

		c_richardson_lucy(c_noise *, const unsigned, const unsigned, const unsigned);

		Mat operator()(const Mat &);

};

Mat preprocess(const Mat & src)
{
	const unsigned height = src.rows, width = src.cols;
	unsigned v, u, q;
	Mat dst, t_dst;
	c_gauss_noise noise(0.5);
	c_richardson_lucy rn(& noise, 3, 3, 100);
//	c_wiener rn(& noise, 3, 3, height, width, 0.1);

	/*
	double a, b, x_1, x_2;
	vector<Mat> src_ch, dst_ch;

	x_1 = 50;
	x_2 = 150;
	a = 255. / (x_2 - x_1);
	b = -a * x_1;

	split(src, src_ch);

	for(auto cur_ch : src_ch)
	{
		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
			{
				uint8_t & pixel = cur_ch.at<uint8_t>(v, u);

				pixel = (pixel <= x_1) ? 0 : (pixel >= x_2) ? 255 : a * pixel + b;
			}

		dst_ch.push_back(cur_ch);
	}

	merge(dst_ch, t_dst);

	medianBlur(t_dst, dst, 7);
	*/

	medianBlur(src, dst, 7);

	Mat t_t_dst(height, width, CV_32FC3);

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			Vec3f & p_f = t_t_dst.at<Vec3f>(v, u);
			Vec3b p_u = dst.at<Vec3b>(v, u);

			p_f[0] = p_u[0] / 255.;
			p_f[1] = p_u[1] / 255.;
			p_f[2] = p_u[2] / 255.;
		}

	cvtColor(t_t_dst, t_dst, CV_BGR2HSV);

	const double alpha = 30, beta = 180;
	const double k_1 = 2.5, k_2 = 0.7;
	const double A = (k_2 - k_1) / (beta - alpha);
	const double B = k_1 - A * alpha;

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			Vec3f & pixel = t_dst.at<Vec3f>(v, u);

			unsigned q;

			pixel[1] *= 2; // 0.2;
			pixel[2] *= 2;
	//		pixel[1] = 0.7;
	//		pixel[2] = 0.5;

			pixel[0] = (A * pixel[0] + B) * pixel[0];
			pixel[0] = pixel[0] - trunc(pixel[0] / 360) * 360;

			for(q = 1; q < 3; q++)
			{
				if(pixel[q] < 0)
					pixel[q] = 0;
				else if(pixel[q] > 1)
					pixel[q] = 1;
			}
		}

	cvtColor(t_dst, dst, CV_HSV2BGR);

	t_dst.create(height, width, CV_8UC3);

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			Vec3f pixel = dst.at<Vec3f>(v, u);
			Vec3b & pixel_u = t_dst.at<Vec3b>(v, u);

			for(q = 0; q < 3; q++)
				pixel_u[q] = pixel[q] * 255;
		}

	bilateralFilter(t_dst, dst, -1, 20, 5);
	dst = rn(dst);

	return dst;
}

c_gauss_noise::c_gauss_noise(const double t_sigma) :
	sigma(t_sigma), sigma_2_2(2 * sigma * sigma), pi_sigma_2_2(M_PI * 2 * sigma * sigma)
{
	;
}

double c_gauss_noise::operator()(const double y, const double x)
{
	return exp(- (x * x + y * y) / sigma_2_2) / pi_sigma_2_2;
}

c_deblurring_kernel::c_deblurring_kernel(c_noise * t_noise, const unsigned t_kernel_height, const unsigned t_kernel_width) :
	kernel_height(t_kernel_height), kernel_width(t_kernel_width)
{
	int v, u;
	const int k_h_2 = kernel_height / 2, k_w_2 = kernel_width / 2;

	kernel.create(kernel_height, kernel_width, CV_64F);

	for(v = 0; v < kernel_height; v++)
		for(u = 0; u < kernel_width; u++)
			kernel.at<double>(v, u) = (* t_noise)(v - k_h_2, u - k_w_2);
}

c_wiener::c_wiener(c_noise * t_noise, const unsigned t_kernel_height, const unsigned t_kernel_width, const unsigned height, const unsigned width, const double K) :
	c_deblurring_kernel(t_noise, t_kernel_height, t_kernel_width)
{
	int v, u;
	Mat kernel_big, kernel_ft, kernel_real;

	// normalize(kernel.clone(), kernel);

	kernel_big = Mat::zeros(height, width, CV_64F);

	for(v = 0; v < kernel_height; v++)
		for(u = 0; u < kernel_width; u++)
			kernel_big.at<double>(v, u) = kernel.at<double>(v, u);

	vector<Mat> kernel_2 = { kernel_big, Mat::zeros(height, width, CV_64F) };

	merge(kernel_2, kernel);
	dft(kernel, kernel_ft, DFT_COMPLEX_OUTPUT);

	kernel.create(height, width, CV_64FC2);

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			Vec2d t_kft = kernel_ft.at<Vec2d>(v, u);
			complex<double> kft(t_kft[0], t_kft[1]), kft_2, k;

			kft_2 = kft * conj(kft);
			k = kft_2 / (kft * (kft_2 + K));

			kernel.at<Vec2d>(v, u) = Vec2d(real(k), imag(k));
		}
}

Mat c_wiener::operator()(const Mat & src)
{
	const unsigned height = src.rows, width = src.cols;
	Mat src_real, src_ft, dst_ft, dst, ret(height, width, CV_8U);
	vector<Mat> dst_2, src_2 = { Mat_<double>(src), Mat::zeros(height, width, CV_64F) };

	merge(src_2, src_real);
	dft(src_real, src_ft, DFT_COMPLEX_OUTPUT);
	mulSpectrums(src_ft, kernel, dst_ft, 0);
	idft(dst_ft, dst, DFT_SCALE);
	split(dst, dst_2);
	dst_2[0].convertTo(dst, CV_8U);
	
	return dst;
}

c_richardson_lucy::c_richardson_lucy(c_noise * t_noise, const unsigned t_kernel_height, const unsigned t_kernel_width, const unsigned t_max_iter) :
	c_deblurring_kernel(t_noise, t_kernel_height, t_kernel_width), max_iter(t_max_iter)
{
	flip(kernel, kernel_flip, -1);
}

Mat c_richardson_lucy::operator()(const Mat & src)
{
	int iter;
	const unsigned height = src.rows, width = src.cols;
	Mat src_real, step_1(height, width, CV_64FC3), step_2(height, width, CV_64FC3), step_3(height, width, CV_64FC3), dst(height, width, CV_64FC3), ret(height, width, CV_8UC3);

	src.convertTo(src_real, CV_64F);
	dst = src_real.clone();

	for(iter = 0; iter < max_iter; iter++)
	{
		printf("Итерация %d из %d = %.2lf %%\n", iter, max_iter, (iter + 1) * 100. / max_iter);

		filter2D(dst, step_1, -1, kernel);
		step_2 = src_real / step_1;
		filter2D(step_2, step_3, -1, kernel_flip);
		dst = dst.mul(step_3);
	}

	dst.convertTo(ret, CV_8UC3);

	return ret;
}

