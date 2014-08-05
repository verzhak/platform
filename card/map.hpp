
#ifndef MAP_HPP
#define MAP_HPP

#include "all.hpp"

class CMap
{
	public:

		const unsigned height, width;

		CMap(const string map_fname, const string height_map_fname);
		~CMap();

		void operator()(uint8_t * buf, const double x, const double y, const double h, const double course, const double roll, const double pitch, const ECoordType coord_system, const double aspect_x, const double aspect_y);
};

#endif

