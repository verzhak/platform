
#include <cstdint>
#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "exception.hpp"

using namespace std;
using namespace cv;

struct koef_line
{
	float a;
	float b;
};

koef_line MNK (Mat *binimage)
{   koef_line koef;
    float n = 0;
	float summx = 0;
	float summy = 0;
	float summxy = 0;
	float summx2 = 0;
	for ( int y = 0; y < binimage->rows; y++ )
	{
		uchar *pbuf = (uchar *)( binimage->data + y * binimage->step );
	    for ( int x = 0; x < binimage->cols; x++ )      
			if ( pbuf[x] > 50 )
			{   
				n++;
				summx+= x;
				summy+= y;
				summxy+= x*y;
				summx2+= x*x;
			}
	}
	koef.a = ( n*summxy - summx*summy ) / ( n*summx2 - summx*summx );
	koef.b = ( summy - koef.a*summx ) / n;
	return koef; 
}


int process_init(int * is_opencv_interface)
{
	int ret = 0;

	try
	{
		throw_null(is_opencv_interface);

		* is_opencv_interface = 1;
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

int process_destroy()
{
	return 0;
}

int process_memory(const uint8_t * src, uint8_t * dst, const unsigned height, const unsigned width, const unsigned channels)
{
	int ret = 0;

	try
	{
		unsigned v, u;
		Mat __src(height, width, CV_8UC3), __dst(height, width, CV_8U);

		throw_if(channels != 3); // Костыль

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
			{
				Vec3b pixel;

				pixel[0] = src[v * width * channels + u * channels];
				pixel[1] = src[v * width * channels + u * channels + 1];
				pixel[2] = src[v * width * channels + u * channels + 2];

				__src.at<Vec3b>(v, u) = pixel;
			}

		throw_if(process_opencv(& __src, & __dst));

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
				dst[v * width + u] = __dst.at<uint8_t>(v, u);
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

int process_opencv(const void * src, void * dst)
{
	int ret = 0;

	try
	{
		const Mat * __src = (const Mat *) src;
		Mat * __dst = (Mat *) dst;
	//--------------------------------------------------------------------------------	
	    Mat greyOriginal,
			Temp,
			Temp1, 
			Temp2;
		Mat element = getStructuringElement(MORPH_RECT, Size(3,3), Point(-1,-1));
		double 	threshold1, 
				threshold2,
				count,
				fullCount,
				ratio;
			
		double 	mu = 0,
				scale = 0;
		double 	mu1 = 0,
				q1 = 0;
		double 	max_sigma = 0,
				max_val = 0;
				
		const int N = 255;
		int 	i = 0,
				j = 0,
				h[N] = {0},
				scale_max = 0;

		// Читаем изображение и конвертируем в градации серого
		cvtColor(*__src, greyOriginal, CV_RGB2GRAY);

		// Получение первого порога
		greyOriginal.copyTo(Temp);
		threshold1 = threshold(Temp, Temp, 120, 200, CV_THRESH_OTSU);

		// Поиск соотношения "Земля - Небо"
		count = countNonZero(Temp);
		fullCount = Temp.cols * Temp.rows;
		ratio = count / fullCount;
		if(ratio > 0.17)
		{
			greyOriginal.copyTo(Temp);
			greyOriginal.copyTo(Temp1);

			// Получаем второй порог и бинаризируем по двум порогам
			
			for( i = 0; i < Temp1.rows; i++ )
			{
				for( j = 0; j <= Temp1.cols; j += 1 )
				{
					uchar *pbuf = (uchar *)( Temp1.data + i * Temp1.cols );
					if ( pbuf[j] < threshold1 )
					{
						scale_max++;
						h[pbuf [j]]++;
					}
				}
			}
			scale = 1./scale_max;
			
			for( i = 0; i < threshold1; i++ )
			{
				mu += i * (double)h[i];
				mu *= scale;
			}
			
			for ( i = 0; i < threshold1; i++ )
			{
				double p_i, q2, mu2, sigma;
				p_i = h[i]*scale;
				mu1 *= q1;
				q1 += p_i;
				q2 = 1. - q1;

				if( std::min(q1,q2) < FLT_EPSILON || std::max(q1,q2) > 1. - FLT_EPSILON )
					continue;

				mu1 = (mu1 + i*p_i)/q1;
				mu2 = (mu - q1*mu1)/q2;
				sigma = q1 * q2 * (mu1 - mu2) * (mu1 - mu2);
				if( sigma > max_sigma )
				{
					max_sigma = sigma;
					max_val = i;
				}
			}

			threshold2 = max_val; // Сохраним результат вычислений второго порога
			greyOriginal.copyTo(Temp2);
			threshold(Temp2, Temp2, threshold1, threshold2, CV_THRESH_BINARY);
//          dilate(Temp1, Temp1, element, cv::Point(-1,-1),10);
            dilate(Temp1, Temp1, element);
//          erode(Temp1, Temp1, element, cv::Point(-1,-1),10);
			erode(Temp1, Temp1, element);
			Canny(Temp1, Temp1, 50, 200);

			// Вычисление коэффициентов для прямой линии горизонта
			koef_line horizonLine;

			horizonLine = MNK(&Temp1);

			int y1 = std::min( (int) horizonLine.b , Temp1.rows );
			int y2 = std::min( (int)(horizonLine.a * (Temp1.cols - 1) + horizonLine.b ) , Temp1.rows);
			float val_thres = 1.7 * (threshold2 - threshold1) / std::min( y1, y2 );

			uchar *ImData = Temp.data;
			for (int y_element = 0; y_element < Temp.rows; y_element++)
			{
				uchar *pbuf = (uchar *)( Temp.data + y_element * Temp.cols );
				for ( int x_element = 0; x_element < Temp.cols; x_element++ )
				{
					int y_line = horizonLine.a * x_element + horizonLine.b;
					int l = abs( y_element - y_line );
	
					int porog = (int)( threshold2 - (l * val_thres) + 0.5 );
					if ( porog < threshold1 ) porog = threshold1;
					if ( pbuf[x_element] > porog ) ImData[y_element * Temp.cols + x_element] = 255;
					else ImData[y_element * Temp.cols + x_element] = 0;
				}
			}
			Temp.data = ImData;
//        	add(Temp,Temp2,Temp);
		}
		Temp1.copyTo(*__dst);
	//--------------------------------------------------------------------------------	
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

