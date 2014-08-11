
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
	CConfig config = CConfig::load_from_file(argv[1]);
	CVideo video(
		QString::fromStdString(config["main"]["video"].value),
		QString::fromStdString(config["main"]["metadata"].value));
	CProcess process(QString::fromStdString(config["main"]["module"].value));
	CCommand command(
		QHostAddress(QString::fromStdString(config["card"]["address"].value)), config["card"]["port"].uint(),
		QHostAddress(QString::fromStdString(config["arinc"]["address"].value)), config["arinc"]["port"].uint(),
		config["main"]["block_size"].uint());

	while(1)
	{
		if(! video(src, metadata))
			break;

		dst = process(src);

		if(! command(dst, metadata))
			break;

		imshow("src", src);
		imshow("dst", dst);

		waitKey(40);
	}

	amv_destroy();

	return 0;
}

