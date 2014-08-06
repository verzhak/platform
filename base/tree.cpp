
#include "tree.hpp"

CTree::CTree()
{
	;
}

CTree::CTree(const string fname)
{
	FILE * fl = NULL;

	try
	{
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

		fclose(fl);
		fl = NULL;

		// ############################################################################ 
		
		xml_document<> doc;
		doc.parse<0>((char *) p_buf);
		xml_node<> * root = doc.first_node("platform");

		unpack(root);
	}
	catch(...)
	{
		if(fl != NULL)
			fclose(fl);

		throw_;
	}
}

CTree & CTree::append(const string ind)
{
	throw_if(childs.find(ind) != childs.end());
	childs[ind] = CTree();

	return childs[ind];
}

CTree & CTree::operator[](const string ind)
{
	throw_if(childs.find(ind) == childs.end());

	return childs[ind];
}

void CTree::pack(xml_document<> & doc, xml_node<> * node)
{
	if(childs.size())
	{
		for(auto & child : childs)
		{
			xml_node<> * sub_node = doc.allocate_node(node_element, child.first.c_str(), child.second.value.c_str());
			node->append_node(sub_node);

			child.second.pack(doc, sub_node);
		}
	}
}

void CTree::unpack(xml_node<> * node)
{
	xml_node<> * next = node->first_node();

	if(next && ! string(next->name()).empty())
	{
		for(; next; next = next->next_sibling())
			append(next->name()).unpack(next);
	}
	else
		value = next->value();
}

string CTree::pack()
{
	stringstream stream;
	xml_document<> doc;
	xml_node<> * root = doc.allocate_node(node_element, "platform");

	doc.append_node(root);

	pack(doc, root);

	stream << * doc.first_node();
	
	return stream.str();
}

