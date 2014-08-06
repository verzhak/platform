
#ifndef TREE_HPP
#define TREE_HPP

#include "base/all.hpp"
#include "3dpart/rapidxml/rapidxml_print.hpp"
#include "3dpart/rapidxml/rapidxml.hpp"

using namespace rapidxml;

class CTree
{
	map<string, CTree> childs;

	protected:

		void pack(xml_document<> & doc, xml_node<> * node);
		void unpack(xml_node<> * node);

	public:

		string value;

		CTree();
		CTree(const string fname);

		string pack();
		CTree & append(const string ind);
		CTree & operator[](const string ind);

		inline const char * c_str() { return value.c_str(); };
		inline unsigned uint() { return atoi(c_str()); };
		inline double real() { return atof(c_str()); };
};

typedef CTree CConfig;

#endif

