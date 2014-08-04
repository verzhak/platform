
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "base/all.hpp"

#include <QtCore>
#include <QXmlSimpleReader>
#include <QUdpSocket>
#include <QMap>

class CElement : public QVariant
{
	public:

		QMap<QString, CElement> elems;

		inline CElement & operator[](const QString ind) { return elems[ind]; };
};

class CHandler : public QXmlDefaultHandler
{
	CElement & __elem;
	QStack<QString> ind;

	public:

		CHandler(CElement & elem);

		bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
		bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
		bool characters(const QString & value);
};

class CConfig
{
	CElement elem;

	public:

		CConfig(const QString fname);

		inline CElement operator[](const QString ind) { return elem[ind]; };
};

#endif

