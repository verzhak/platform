
#include "map.hpp"

CMap::CMap(const string map_fname, const string height_map_fname) :
	height(1050), width(1400)
{
	throw_if(combine_init(
		height, width,
		1024, // texture_size
		4, // texture_per_row
		EMA_LOW,
		EP_BLACK_WHITE,
		true, // is_polygon_as_a_line
		false, // is_print_coord
		map_fname.c_str(),
		height_map_fname.c_str(),
		"",
		"/usr/share/fonts/TTF/times.ttf"
		));
}

CMap::~CMap()
{
	combine_destroy();
}

void CMap::operator()(uint8_t * buf, const double x, const double y, const double h, const double course, const double roll, const double pitch, const ECoordType coord_system, const double aspect_x, const double aspect_y)
{
	throw_if(combine_set_camera(x, y, h, course, roll, pitch, coord_system));
	throw_if(combine_set_aspect(aspect_x, aspect_y));
	throw_if(combine_redraw_read_frame(buf));
}

