
#ifndef MAP_HPP
#define MAP_HPP

#include "all.hpp"
#include "matrix.hpp"

class CMyMap
{
	CSXFMap __map;

	public:

		CMyMap(const string map_fname, const string height_map_fname, const vector<unsigned> codes);

		CMatrix operator()(const double x, const double y, const double h, const double course, const double roll, const double pitch, const double aspect_x, const double aspect_y);

		inline vector< vector<CVertex> > contours(unsigned & pnts_num) { return __map.contours(pnts_num); };
};

#endif

