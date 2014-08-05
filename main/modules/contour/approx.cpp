
#include "contour/approx.hpp"

unsigned height, width;

// true, если current_contour подходит для аппроксимации прямой
bool condition(vector<Point> cnt, const Point pnt)
{
	// По расстоянию до прямой

	if(cnt.size() < 2)
		return true;

	const double T = 30 * log(1 + pnt.y) / log(height);
	double A, B, C;
	Point p_0 = cnt[0], p_1 = * (cnt.end() - 1);
	
	if(p_0.x == p_1.x)
	{
		A = 1;
		B = 0;
		C = - p_0.x;
	}
	else
	{
		A = (p_1.y - p_0.y) / (p_1.x - p_0.x);
		B = -1;
		C = p_0.y - A * p_0.x;
	}

	return (fabs(A * pnt.x + B * pnt.y + C) / sqrt(A * A + B * B)) < T;
}

void append_contour(vector<vector<Point> > & contours, const vector<Point> cnt)
{
	if(cnt.size() > 10)
		contours.push_back(cnt);
}

bool cmp(vector<Point> op_1, vector<Point> op_2)
{
	return (op_1.size() < op_2.size());
}

// ############################################################################

void next_point(const Mat & src, Mat & mask, vector<Point> & cnt, const Point pnt)
{
	int v, u;
	Point next_pnt;

	if(src.at<uint8_t>(pnt) && mask.at<uint8_t>(pnt) && condition(cnt, pnt))
	{
		cnt.push_back(pnt);
		mask.at<uint8_t>(pnt) = 0;

		for(v = -1; v <= 1; v++)
			for(u = -1; u <= 1; u++)
			{
				next_pnt = Point(pnt.x + u, pnt.y + v);

				if(next_pnt.x >= 0 && next_pnt.x < width && next_pnt.y >= 0 && next_pnt.y < height)
					next_point(src, mask, cnt, next_pnt);
			}
	}
}

double euclid(const Point p_1, const Point p_2)
{
	return sqrt(pow(p_1.x - p_2.x, 2) + pow(p_1.y - p_2.y, 2));
}

Mat approx_(const Mat & src)
{
	height = src.rows;
	width = src.cols;

	unsigned v, u, size;
	double d, max_d;
	vector<Point> cnt, t_cnt;
	vector<vector<Point> > contours, dst_contours;
	Point max_v, max_u;
	Mat mask = Mat::ones(height, width, CV_8U), dst = Mat::zeros(height, width, CV_8UC3);

	// ############################################################################ 
	// Векторизация

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			cnt.clear();
			next_point(src, mask, cnt, Point(u, v));
			append_contour(contours, cnt);
		}

	// ############################################################################ 
	// Упрощение

	const unsigned step = 0xFFFF / (contours.size() + 1);
	
	for(auto cnt : contours)
	{
		convexHull(cnt, t_cnt);
		size = t_cnt.size();

		// TODO Может, проще?
		for(v = 0; v < size; v++)
			for(u = v + 1; u < size; u++)
			{
				d = euclid(t_cnt[v], t_cnt[u]);

				if((! v && u == 1) || (d > max_d))
				{
					max_v = t_cnt[v];
					max_u = t_cnt[u];
					max_d = d;
				}
			}

		t_cnt.clear();
		t_cnt.push_back(max_v);
		t_cnt.push_back(max_u);

		dst_contours.push_back(t_cnt);
	}

	// ############################################################################ 
	// Отрисовка

	size = dst_contours.size();

	for(v = 0; v < size; v++)
		line(dst, dst_contours[v][0], * (dst_contours[v].end() - 1), to_color<Scalar>((((v + 1) * step) << 8) + 0xFF), 2);

	return dst;
}

// ############################################################################ 

void approx(const vector<string> & argv)
{
	Mat src, dst;

	throw_if(argv.size() != 1);

	src = imread(argv[0], CV_LOAD_IMAGE_GRAYSCALE);
	throw_null(src.data);

	dst = approx_(src);
}

