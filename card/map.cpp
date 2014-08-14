
#include "map.hpp"

CMyMap::CMyMap(const string map_fname, const string height_map_fname, const vector<unsigned> codes)
{
	__map.load(map_fname, height_map_fname, codes);
}

CMatrix CMyMap::operator()(const double x, const double y, const double h, const double course, const double roll, const double pitch, const double aspect_x, const double aspect_y)
{
	CMatrix __mtx = CMatrix::eye();

	__mtx.pitch(pitch);
	__mtx.roll(roll);
	__mtx.course(course);
	__mtx.translate(- x, - h, - y);

	return __mtx;
}

