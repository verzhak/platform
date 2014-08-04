
#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "all.hpp"

class CVideo
{
	VideoCapture video_fl;
	QFile metadata_fl;
	QXmlStreamReader stream;

	public:

		CVideo(const QString video_fname, const QString metadata_fname);

		bool operator()(Mat & frame, QMap<QString, QVariant> & metadata);
};

#endif

