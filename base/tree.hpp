
#ifndef TREE_HPP
#define TREE_HPP

#include "base/all.hpp"
#include "3dpart/rapidxml/rapidxml_print.hpp"
#include "3dpart/rapidxml/rapidxml.hpp"

using namespace rapidxml;

class CTree
{
	map<string, CTree> childs;

	void add(CTree & node, const CTree & add_node);

	protected:

		void pack(xml_document<> & doc, xml_node<> * node) const;

	public:

		static const string begin_tag, end_tag;

		string value;

		CTree();

		string pack() const;
		CTree & append(const string ind);
		CTree & operator[](const char * ind) { return (* this)[string(ind)]; };
		CTree & operator[](const string ind);
		CTree & get(const string ind);

		inline const char * c_str() const { return value.c_str(); };
		inline bool exist(const string ind) const { return childs.find(ind) != childs.end(); };

		void operator=(const string __value) { value = __value; };
		void operator=(const unsigned __value);
		void operator=(const double __value);
		CTree operator+(const CTree & op);

		inline unsigned uint() const { return atoi(value.c_str()); };
		inline double real() const { return atof(value.c_str()); };

		static CTree load_from_file(const string fname);
		static CTree load_from_string(const char * buf);
};

typedef CTree CConfig;

#endif

