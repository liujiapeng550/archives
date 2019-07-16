#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <array>
#include <functional>
#include "convert.h"

using namespace std;

void convert_cube_uv_to_xyz(int index, float u, float v, float *x, float *y, float *z)
{
	// convert range 0 to 1 to -1 to 1
	float uc = 2.0f * u - 1.0f;
	float vc = 2.0f * v - 1.0f;
	switch (index)
	{
	case 0: *x = 1.0f; *y = vc; *z = -uc; break;	// POSITIVE X  right
	case 1: *x = -1.0f; *y = vc; *z = uc; break;	// NEGATIVE X  left
	case 2: *x = uc; *y = 1.0f; *z = -vc; break;	// POSITIVE Y  top 
	case 3: *x = uc; *y = -1.0f; *z = vc; break;	// NEGATIVE Y  bottom 
	case 4: *x = uc; *y = vc; *z = 1.0f; break;	// POSITIVE Z    front
	case 5: *x = -uc; *y = vc; *z = -1.0f; break;	// NEGATIVE Z back
	}
}


Cubemap::Cubemap(std::array<std::string, 6> image_filenames)
	:image_files(image_filenames)
{
}


namespace{

inline unsigned char uchar_color(float c)
{
	return (unsigned char)max(min(int(c * 255), 255), 0);
}
};



std::ostream& operator<<(std::ostream& os, RGB color)
{
	os << color.r << '\t' << color.g << '\t' << color.b;
	return os;
}