
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "base/all.hpp"
#include "3dpart/rapidxml/rapidxml.hpp"

using namespace rapidxml;

class CElement
{
	public:

		string value;
		map<string, CElement> elems;

		inline CElement & operator[](const string ind) { return elems[ind]; };
		inline unsigned to_uint() { return atoi(value.c_str()); };
};

class CConfig
{
	CElement elem;

	void next_node(xml_node<> * current, vector<string> ind);

	public:

		CConfig(const string fname);

		inline CElement operator[](const string ind) { return elem[ind]; };
};

#endif

