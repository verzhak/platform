
#include "config.hpp"

CHandler::CHandler(CElement & elem) :
	QXmlDefaultHandler(), __elem(elem)
{
	;
}

bool CHandler::startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts)
{
	if(qName != "platform")
		ind.push(qName);

	return true;
}

bool CHandler::endElement(const QString & namespaceURI, const QString & localName, const QString & qName)
{
	if(ind.size())
		ind.pop();

	return true;
}

bool CHandler::characters(const QString & value)
{
	if(! value.trimmed().isEmpty())
	{
		int v;
		CElement * cur = & __elem;

		for(v = 0; v < ind.size(); v++)
		{
			if(! cur->elems.contains(ind[v]))
				cur->elems[ind[v]] = CElement();

			cur = & cur->elems[ind[v]];
		}

		cur->setValue(value);
	}

	return true;
}

CConfig::CConfig(const QString fname)
{
	QFile fl(fname);

	throw_if(! fl.open(QIODevice::ReadOnly | QIODevice::Text));

	QXmlSimpleReader reader;
	QXmlInputSource source(& fl);
	CHandler handler(elem);

	reader.setContentHandler(& handler);
	reader.setErrorHandler(& handler);

	throw_if(! reader.parse(& source));
}

