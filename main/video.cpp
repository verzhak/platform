
#include "video.hpp"

CVideo::CVideo(const QString video_fname, const QString metadata_fname) :
	video_fl(video_fname.toStdString()), metadata_fl(metadata_fname)
{
	throw_if(! video_fl.isOpened());
	throw_if(! metadata_fl.open(QIODevice::ReadOnly | QIODevice::Text));

	stream.setDevice(& metadata_fl);

	throw_if(! stream.readNextStartElement());
	throw_if(stream.name() != "amv");
}

bool CVideo::operator()(Mat & frame, QMap<QString, QVariant> & metadata)
{
	if(! video_fl.read(frame))
		return false;
	
	auto value_to_QVariant = [](QStringRef & type, QStringRef & value)
	{
		QVariant ret;

		if(type == "bool")
			ret = QVariant(value == "true");
		else if(type == "double")
			ret = QVariant(value.toDouble());
		else if(type == "int")
			ret = QVariant(value.toInt());
		else if(type == "unsigned")
			ret = QVariant(value.toUInt());
		else
			ret = QVariant(value.toString());

		return ret;
	};

	if(stream.readNextStartElement() && stream.name() == "frame")
	{
		bool is_frame = true;
		QStringRef value_name, value_type;

		while(is_frame)
		{
			const QXmlStreamReader::TokenType type = stream.readNext();
			QStringRef name = stream.name();
			QStringRef value = stream.text();
			
			switch(type)
			{
				case QXmlStreamReader::StartElement:
				{
					if(stream.isWhitespace())
						stream.skipCurrentElement();

					value_name = name;
					value_type = stream.attributes().value("type"); // If type attribute not exists, return empty string

					break;
				}
				case QXmlStreamReader::EndElement:
				{
					if(name == "frame")
						is_frame = false;

					break;
				}
				case QXmlStreamReader::Characters:
				{
					if(! stream.isWhitespace())
						metadata[value_name.toString()] = value_to_QVariant(value_type, value);

					break;
				}
			}
		}
	}
	else
		return false;

	return true;
}

