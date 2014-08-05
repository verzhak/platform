
#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <cstdio>

#define throw_ \
{\
	std::fprintf(stderr, "[Exception] File %s, line %d\n", __FILE__, __LINE__);\
	throw 1;\
};

#define throw_if(condition) \
{\
	if((condition))\
		throw_;\
}

#define throw_null(pointer) \
	throw_if((pointer) == NULL)

#endif

