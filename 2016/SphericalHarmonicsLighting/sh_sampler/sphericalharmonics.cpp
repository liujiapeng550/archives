#define _USE_MATH_DEFINES
#include <math.h>
#include "sphericalharmonics.h"
#include "CommonMacro.h"
using namespace std;

const float PI = float(M_PI);

template class HarmonicIntegral<4>;
template class HarmonicIntegral<9>;
template class HarmonicIntegral<16>;


// reference: https://en.wikipedia.org/wiki/Table_of_spherical_harmonics

template<>
void HarmonicBasis<4>(float basis[4], float x, float y, float z, float r)
{
	basis[0] = 1.f / 2.f * sqrt(1.f / PI);
	basis[1] = Y1_1(y);
	basis[2] = Y10(z);
	basis[3] = Y11(x);
}


template<>
void HarmonicBasis<9>(float basis[9], float x, float y, float z, float r)
{
	HarmonicBasis<4>(basis, x, y, z, r);
	basis[4] = Y2_2(x, y);
	basis[5] = Y2_1(y, z);
	basis[6] = Y20(z);
	basis[7] = Y21(x, z) ;
	basis[8] = Y22(x, y) ;
}

template<>
void HarmonicBasis<16>(float basis[16], float x, float y, float z, float r)
{
	HarmonicBasis<9>(basis, x, y, z, r);
	basis[9] = Y3_3(x, y);
	basis[10] = Y3_2(x, y, z) ;
	basis[11] = Y3_1(y, z));
	basis[12] = Y30(z)) ;
	basis[13] = Y31(x, z));
	basis[14] = Y32(x, y, z);
	basis[15] = Y33(x, y) ;

}

template<int COEF_NUM>
HarmonicIntegral<COEF_NUM>::HarmonicIntegral()
{
	for (int i = 0; i < COEF_NUM; i++)
		coef[i] = { 0,0,0 };
}


template<int COEF_NUM>
void HarmonicIntegral<COEF_NUM>::operator()(XYZRGB pixel)
{
	float basis[COEF_NUM];
	float x = pixel.pos.x, y = pixel.pos.y, z = pixel.pos.z;
	float r = sqrt(x*x + y * y + z * z);
	HarmonicBasis<COEF_NUM>(basis, x, y, z, r);
	for (int i = 0; i < COEF_NUM; i++) {
		coef[i].r += pixel.color.r*basis[i];
		coef[i].g += pixel.color.g*basis[i];
		coef[i].b += pixel.color.b*basis[i];
	}
	count++;
}

template<int COEF_NUM>
std::array<RGB, COEF_NUM> HarmonicIntegral<COEF_NUM>::getCoefficients()
{
	std::array<RGB, COEF_NUM> coefficients = coef;

	for (auto& c : coefficients) {
		c.r = c.r * 4 * PI / count;
		c.g = c.g * 4 * PI / count;
		c.b = c.b * 4 * PI / count;
	}
	return coefficients;
}