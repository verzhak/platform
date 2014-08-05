
#include "contour/contour.hpp"

Mat novikov(const Mat & src, const double k, const double max_thresh, const double min_thresh, const double alpha);

Mat sobel(const Mat & src)
{
	Mat dst, src_gray, t_src, t_dst, t_t_dst;

	t_src = preprocess(src);
	cvtColor(t_src, src_gray, CV_RGB2GRAY);

	Canny(src_gray, dst, 0, 70);
			
// 	Sobel(src_gray, t_dst, -1, 1, 1);
//	dst = t_dst;

//	double k = 3, maxt = 100, mint = 70, alpha = 8;
//	dst = novikov(src_gray, k, maxt, mint, alpha);

	// dst = approx_(dst);

	return dst;
}

Mat novikov(const Mat & src, const double k, const double max_thresh, const double min_thresh, const double alpha)
{
	unsigned v, u, t_row, t_col, t_v, t_u, from_t_u, to_t_u, from_t_v, to_t_v, num, d1 = k, k_coef = 2 * (k + 1);
	unsigned height = src.size().height, width = src.size().width;
	unsigned height_k = height - k_coef, width_k = width - k_coef;
	int value_row, value_col, op;
	double sigma, m, d;
	double * p_mod, * mod = new double[height * width];
	MatND hist;
	Mat dst = Mat::zeros(height, width, CV_8U);

	for(v = 0, p_mod = mod; v < height_k; v++, p_mod += k_coef)
		for(u = 0; u < width_k; u++, p_mod++)
		{
			for(t_row = u + k, t_col = v + k, op = 1, value_row = 0, value_col = 0; op <= k; t_row++, t_col++, op++)
			{
				value_row += (src.at<uint8_t>(v, t_row + op) - src.at<uint8_t>(v, t_row - op)) * op;
				value_col += (src.at<uint8_t>(t_col + op, u) - src.at<uint8_t>(t_col - op, u)) * op;
			}

			(* p_mod) = sqrt(value_row * value_row + value_col * value_col);
		}

	for(v = 0, p_mod = mod; v < height; v++)
		for(u = 0; u < width; u++, p_mod++)
		{
			if((* p_mod) < max_thresh)
			{
				if((* p_mod) > min_thresh)
				{
					from_t_v = v;
					from_t_u = u;

					to_t_v = min(height - 1, v + 2 * d1);
					to_t_u = min(width - 1, u + 2 * d1);

					for(num = 0, m = 0, t_v = from_t_v; t_v <= to_t_v; t_v++)
						for(t_u = from_t_u; t_u <= to_t_u; t_u++)
						{
							m += mod[t_v * width + t_u];
							num++;
						}

					m /= num;

					for(sigma = 0, t_v = from_t_v; t_v <= to_t_v; t_v++)
						for(t_u = from_t_u; t_u <= to_t_u; t_u++)
						{
							d = mod[t_v * width + t_u] - m;
							sigma += d * d;
						}

					sigma = sqrt(sigma / num);

					dst.at<uint8_t>(v, u) = (* p_mod) > (m + alpha * sigma) ? 255 : 0;
				}
			}
			else
				dst.at<uint8_t>(v, u) = 255;
		}

	return dst;
}

