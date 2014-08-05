
#include "config.hpp"

void CConfig::next_node(xml_node<> * current, vector<string> ind)
{
	xml_node<> * next = current->first_node();
	const string value = next->value();

	if(next && ! string(next->name()).empty())
	{
		for(; next; next = next->next_sibling())
		{
			ind.push_back(next->name());
			next_node(next, ind);
			ind.pop_back();
		}
	}
	else
	{
		const unsigned size = ind.size();
		unsigned v;
		CElement * cur = & elem;

		for(v = 0; v < size; v++)
		{
			if(cur->elems.find(ind[v]) == cur->elems.end())
				cur->elems[ind[v]] = CElement();

			cur = & cur->elems[ind[v]];
		}

		cur->value = value;
	}
}

CConfig::CConfig(const string fname)
{
	FILE * fl;
	size_t size;
	shared_ptr<uint8_t> buf;
	uint8_t * p_buf;

	throw_null(fl = fopen(fname.c_str(), "r"));
	throw_if(fseek(fl, 0, SEEK_END));
	throw_if((size = ftell(fl)) < 0);
	throw_if(fseek(fl, 0, SEEK_SET));

	buf.reset(new uint8_t[size + 1], std::default_delete<uint8_t[]>());
	throw_null(p_buf = buf.get());

	throw_if(fread(p_buf, 1, size, fl) != size);
	p_buf[size] = '\0';

	// ############################################################################ 
	
	xml_document<> doc;
	doc.parse<0>((char *) p_buf);
	xml_node<> * root = doc.first_node("platform");

	next_node(root, vector<string>());
}

