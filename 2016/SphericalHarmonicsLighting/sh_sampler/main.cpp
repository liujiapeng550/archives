#include <iostream>
#include <array>
#include <string>
#include "convert.h"
#include "sphericalharmonics.h"

using namespace std;

int main(int argc, char *argv[])
{
	/*if (argc < 8){
		cout << "Usage: sh_sampler posx negx posy negy posz negz output.txt" << endl;
		return 0;
	}*/
	
	try{
		cout << "sampling ..." << endl;
		array<string, 6> images;
		//for (int i = 0; i < 6; i++){
	/*		images[0] = "..//data//posx.jpg";
			images[1] = "..//data//negx.jpg";
			images[2] = "..//data//posy.jpg";
			images[3] = "..//data//negy.jpg";
			images[4] = "..//data//posz.jpg";
			images[5] = "..//data//negz.jpg";*/

			images[0] = "..//data//right.jpg";
			images[1] = "..//data//left.jpg";
			images[2] = "..//data//top.jpg";
			images[3] = "..//data//bottom.jpg";
			images[4] = "..//data//front.jpg";
			images[5] = "..//data//back.jpg";
		//}
		string outputfile = "..//data//sh_coefficient.txt";
		Cubemap cubemap(images);
		//WritePLY plywriter(plyfile, cubemap.size());
		//cubemap.Read(plywriter);
		HarmonicIntegral<16> harmonics;
		cubemap.Read(harmonics);
		auto coefs = harmonics.getCoefficients();
		ofstream ofs(outputfile);
		for (auto c : coefs){
			ofs << c << endl;
			cout << c << endl;
		}
		cout << "written to " + outputfile << endl;
	}
	catch (const exception& e){
		cerr << e.what() << endl;
	}
	return 0;
}