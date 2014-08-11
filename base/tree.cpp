
#include "tree.hpp"

const string CTree::begin_tag = "<platform>";
const string CTree::end_tag = "</platform>";

CTree::CTree()
{
	;
}

CTree & CTree::append(const string ind)
{
	throw_if(exist(ind));
	childs[ind] = CTree();

	return childs[ind];
}

CTree & CTree::operator[](const string ind)
{
	throw_if(! exist(ind));

	return childs[ind];
}

CTree & CTree::get(const string ind)
{
	if(exist(ind))
		return (* this)[ind];

	return append(ind);
}

void CTree::pack(xml_document<> & doc, xml_node<> * node) const
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

string CTree::pack() const
{
	stringstream stream;
	xml_document<> doc;
	xml_node<> * root = doc.allocate_node(node_element, "platform");

	doc.append_node(root);

	pack(doc, root);

	stream << * doc.first_node();
	
	return stream.str();
}

void CTree::operator=(const unsigned __value)
{
	stringstream stream;

	stream << __value;
	value = stream.str();
}

void CTree::operator=(const double __value)
{
	stringstream stream;

	stream << __value;
	value = stream.str();
}

void CTree::add(CTree & node, const CTree & add_node)
{
	for(auto & elem : add_node.childs)
	{
		CTree & child = node.get(elem.first);

		add(child, elem.second);
	}

	node.value = add_node.value;
}

CTree CTree::operator+(const CTree & op)
{
	CTree ret;

	add(ret, (* this));
	add(ret, op);
	
	return ret;
}

CTree CTree::load_from_file(const string fname)
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

		return load_from_string((char *) p_buf);
	}
	catch(...)
	{
		if(fl != NULL)
			fclose(fl);

		throw_;
	}
}

CTree CTree::load_from_string(const char * buf)
{
	char * t_buf;
	
	throw_null(t_buf = strdup(buf));

	xml_document<> doc;
	doc.parse<0>(t_buf);
	xml_node<> * root = doc.first_node("platform");
	CTree tree;

	function<void(CTree & elem, xml_node<> * node)>
		unpack = [ & unpack ](CTree & elem, xml_node<> * node)
	{
		xml_node<> * next = node->first_node();

		if(next && ! string(next->name()).empty())
			for(; next; next = next->next_sibling())
				unpack(elem.append(next->name()), next);
		else
			elem.value = next->value();
	};

	unpack(tree, root);

	return tree;
}

