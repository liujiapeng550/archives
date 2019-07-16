#include <iostream>
#include <array>
#include <string>
#include <FreeImage.h>
#include "convert.h"
#include "sphericalharmonics.h"
#include "glm/glm.hpp"

#include "time.h"
using namespace std;

int main(int argc, char *argv[])
{
	/*if (argc < 8){
		cout << "Usage: sh_sampler posx negx posy negy posz negz output.txt" << endl;
		return 0;
	}*/
	
	
	clock_t start, finish;
	start = clock();
	double duration;
	try {
		cout << "sampling ..." << endl;

		////加载全景图逻辑
		//// load image
		//string envPath = "..//data//env.jpg";
		//int rows, cols;
		//FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(envPath.c_str());
		//FIBITMAP* img = FreeImage_Load(fif, envPath.c_str());
		//if (!img) {
		//	throw runtime_error("load " + envPath + " failed");
		//	cols = FreeImage_GetWidth(img);
		//	rows = FreeImage_GetHeight(img);
		//}
		//glm::vec3 coeffs[16];
		//for (int i = 0; i < 16; ++i) {
		//	coeffs[i] = glm::vec3(0.f);
		//}
		// int ntheta = FreeImage_GetHeight(img);
		// int nphi = FreeImage_GetWidth(img);

		//if (min(ntheta, nphi) > 50u) {
		//	/// Project _InfiniteAreaLight_ to SH from lat-long representation

		//	// Precompute $\theta$ and $\phi$ values for lat-long map projection
		//	float *buf = new float[2 * ntheta + 2 * nphi];
		//	float *bufp = buf;
		//	float *sintheta = bufp;  bufp += ntheta;
		//	float *costheta = bufp;  bufp += ntheta;
		//	float *sinphi = bufp;    bufp += nphi;
		//	float *cosphi = bufp;
		//	for (int theta = 0; theta < ntheta; ++theta) {
		//		sintheta[theta] = sinf((theta + .5f) / ntheta * M_PIf);
		//		costheta[theta] = cosf((theta + .5f) / ntheta * M_PIf);
		//	}
		//	for (int phi = 0; phi < nphi; ++phi) {
		//		sinphi[phi] = sinf((phi + .5f) / nphi * M_2PIf);
		//		cosphi[phi] = cosf((phi + .5f) / nphi * M_2PIf);
		//	}

		//	// SH calculation;
		//	// TODO:拷贝过程可以优化
		//	const float F{ M_2PIf / ntheta * M_2PIf / nphi };
		//	for (int theta = 0; theta < ntheta; ++theta) {
		//		for (int phi = 0; phi < nphi; ++phi) {
		//			// Add _InfiniteAreaLight_ texel's contribution to SH coefficients
		//			// 左手坐标系，z轴向上；
		//			glm::vec3 w{ sintheta[theta] * cosphi[phi], sintheta[theta] * sinphi[phi], costheta[theta] };
		//			/******************************************************************************/
		//			// 上句 w在构建的时候就是单位化的，所以注释掉下面的；
		//			//  w = Normalize(LightToWorld(w));
		//			/******************************************************************************/
		//			glm::vec3 Le;
		//			RGBQUAD c;
		//			FreeImage_GetPixelColor(img, theta, phi, &c);
		//			Le = glm::vec3(c.rgbRed, c.rgbGreen, c.rgbBlue);

		//			coeffs[SHIndex(0, 0)] += Le * Y00*sintheta[theta] * F;
		//			coeffs[SHIndex(1, -1)] += Le * Y1_1(w.y)*sintheta[theta] * F;
		//			coeffs[SHIndex(1, 0)] += Le * Y10(w.z)*sintheta[theta] * F;
		//			coeffs[SHIndex(1, 1)] += Le * Y11(w.x)*sintheta[theta] * F;
		//			coeffs[SHIndex(2, -2)] += Le * Y2_2(w.x, w.y)*sintheta[theta] * F;
		//			coeffs[SHIndex(2, -1)] += Le * Y2_1(w.y, w.z)*sintheta[theta] * F;
		//			coeffs[SHIndex(2, 0)] += Le * Y20(w.z)*sintheta[theta] * F;
		//			coeffs[SHIndex(2, 1)] += Le * Y21(w.x, w.z)*sintheta[theta] * F;
		//			coeffs[SHIndex(2, 2)] += Le * Y22(w.x, w.y)*sintheta[theta] * F;
		//		}
		//	}
		//	/*
		//	float N=1.f/(ntheta*nphi);
		//	coeffs[SHIndex(0,0)]*=N;
		//	coeffs[SHIndex(1,-1)]*=N;
		//	coeffs[SHIndex(1,0)]*=N;
		//	coeffs[SHIndex(1,1)]*=N;
		//	coeffs[SHIndex(2,-2)]*=N;
		//	coeffs[SHIndex(2,-1)]*=N;
		//	coeffs[SHIndex(2,0)]*=N;
		//	coeffs[SHIndex(2,1)]*=N;
		//	coeffs[SHIndex(2,2)]*=N;
		//	*/
		//	int count = ntheta * nphi;
		//	coeffs[SHIndex(0, 0)] *= A0 * 4 * M_PIf / count;
		//	coeffs[SHIndex(1, -1)] *= A1 * 4 * M_PIf / count;
		//	coeffs[SHIndex(1, 0)] *= A1 * 4 * M_PIf / count;
		//	coeffs[SHIndex(1, 1)] *= A1 * 4 * M_PIf / count;
		//	coeffs[SHIndex(2, -2)] *= A2 * 4 * M_PIf / count;
		//	coeffs[SHIndex(2, -1)] *= A2 * 4 * M_PIf / count;
		//	coeffs[SHIndex(2, 0)] *= A2 * 4 * M_PIf / count;
		//	coeffs[SHIndex(2, 1)] *= A2 * 4 * M_PIf / count;
		//	coeffs[SHIndex(2, 2)] *= A2 * 4 * M_PIf / count;

		//	// Free memory used for lat-long theta and phi values

		//}
		//std::array<RGB, 16> coefEnv;
		//int index = 0;
		//for (glm::vec3& c : coeffs) {
		//	coefEnv[index] = { c.r,c.g,c.b };
		//	index++;
		//}
		//string outputEnvfile = "..//data//Env.txt";
		//ofstream ofsEnv(outputEnvfile);
		//for (auto c : coefEnv) {
		//	ofsEnv << c << endl;
		//	ofsEnv << c << endl;
		//}
	//	cout << "written to " + outputEnvfile << endl;

		array<string, 6> images;
		//for (int i = 0; i < 6; i++){
			//images[0] = "..//data//posx.jpg";
			//images[1] = "..//data//negx.jpg";
			//images[2] = "..//data//posy.jpg";
			//images[3] = "..//data//negy.jpg";
			//images[4] = "..//data//posz.jpg";
			//images[5] = "..//data//negz.jpg";

	/*		images[0] = "..//data//right.jpg";
			images[1] = "..//data//left.jpg";
			images[2] = "..//data//top.jpg";
			images[3] = "..//data//bottom.jpg";
			images[4] = "..//data//front.jpg";
			images[5] = "..//data//back.jpg";*/

			images[0] = "..//data//Soft_1light_cuberight.tif";
			images[1] = "..//data//Soft_1light_cubeleft.tif";
			images[2] = "..//data//Soft_1light_cubetop.tif";
			images[3] = "..//data//Soft_1light_cubebottom.tif";
			images[4] = "..//data//Soft_1light_cubefront.tif";
			images[5] = "..//data//Soft_1light_cubeback.tif";
		//}
		string outputfile = "..//data//Soft.txt";
		Cubemap cubemap(images);
		//WritePLY plywriter(plyfile, cubemap.size());
		//cubemap.Read(plywriter);
		HarmonicIntegral<16> harmonics;
		cubemap.Read(harmonics);
		auto coefs = harmonics.getCoefficients();

		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;
		printf("%f seconds/n", duration);
		ofstream ofs(outputfile);
		for (auto c : coefs){
			ofs << c << endl;
			cout << c << endl;
		}
	//	cout << "written to " + outputfile << endl;
	}
	catch (const exception& e){
		cerr << e.what() << endl;
	}
	return 0;
}