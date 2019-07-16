#pragma once
//==========================================================================================
// Spherical Harmonics Declarations
// 左手坐标系，z轴向上
// 下面做了简化，将Y1_1 Y11 Y2_1 Y21的负号省去，因为在通过SH函数与系数计算radiance时候负负得正。
#define M_MATERIAL_ID_NONE -9999
#define SH_L_MAX 2u
#define SHTerms 9u //#define SHTerms(lmax) ((lmax + 1) * (lmax + 1))
#define SHIndex(l, m) ( l*l + l + m)
#define Y00 0.282095f
#define Y1_1(y)  (0.488603f*y)
#define Y10(z)   (0.488603f*z)
#define Y11(x)   (0.488603f*x)
#define Y2_2(x,y) (1.092548f*x*y)
#define Y2_1(y,z) (1.092548f*y*z)
#define Y20(z)   (0.315392f*(3*z*z-1.f))
#define Y21(x,z) (1.092548f*x*z)
#define Y22(x,y) (0.546274f*(x*x-y*y))
#define Y3_3(x,y) (0.590043589927f*y*(3*x*x-y*y))
#define Y3_2(x,y,z) (2.89061144264f*x*y*z)
#define Y3_1(y,z) (0.457045799464f*y*(5*z*z-1.f)
#define Y30(z) (0.37317633259f*z*(5.f*z*z-3.f)
#define Y31(x,z) (0.457045799464f*x*(5.f*z*z-1.f)
#define Y32(x,y,z) (1.44530572132f*z*(x*x-y*y))
#define Y33(x,y) (0.590043589927f*x*(x*x-3.f*y*y))
#define A0 3.141593f
#define A1 2.094395f
#define A2 0.785398f
#define A3 0.f
#define A4 -0.130900f
#define A5 0.f
#define A6 0.049087f
#define M_PIf 3.14159265358979323846f
#define M_2PIf (6.283185307179586476925286766559f)