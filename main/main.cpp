
#include "all.hpp"
#include "video.hpp"
#include "process.hpp"
#include "command.hpp"

int main(const int argc, const char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	Mat src, dst;
	QMap<QString, QVariant> metadata;
	CConfig config(argv[1]);
	CVideo video(config["main"]["video"].toString(), config["main"]["metadata"].toString());
	CProcess process(config["main"]["module"].toString());
	CCommand command(
		QHostAddress(config["card"]["address"].toString()), config["card"]["port"].toUInt(),
		QHostAddress(config["arinc"]["address"].toString()), config["arinc"]["port"].toUInt());

	while(video(src, metadata))
	{
		dst = process(src);

//		command.send_image(dst);
		command.send_orientation(metadata);
//		command.correlation();

		imshow("src", src);
		imshow("dst", dst);

		waitKey(40);
	}

	amv_destroy();

	return 0;
}

