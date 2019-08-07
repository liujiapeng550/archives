#include "Application.h"
using namespace std;

int main(int argc, char *argv[])
{
	try{
		/*if (argc < 9)
			throw runtime_error("Usage: sh_lighting posx negx posy negy posz negz coefficients.txt model");*/
		array<string, 6> cube_textures;
		//for (int i = 0; i < 6; i++)
	/*	cube_textures[0] = "..//data//posx.jpg";
		cube_textures[1] = "..//data//negx.jpg";
		cube_textures[2] = "..//data//posy.jpg";
		cube_textures[3] = "..//data//negy.jpg";
		cube_textures[4] = "..//data//posz.jpg";
		cube_textures[5] = "..//data//negz.jpg";*/

		cube_textures[0] = "..//data//right.jpg";
		cube_textures[1] = "..//data//left.jpg";
		cube_textures[2] = "..//data//top.jpg";
		cube_textures[3] = "..//data//bottom.jpg";
		cube_textures[4] = "..//data//front.jpg";
		cube_textures[5] = "..//data//back.jpg";

		/*cube_textures[0] = "..//data//Soft_1light_cuberight.tif";
		cube_textures[1] = "..//data//Soft_1light_cubeleft.tif";
		cube_textures[2] = "..//data//Soft_1light_cubetop.tif";
		cube_textures[3] = "..//data//Soft_1light_cubebottom.tif";
		//cube_textures[4] = "..//data//Soft_1light_cubefront.tif";
		cube_textures[5] = "..//data//Soft_1light_cubeback.tif";*/

		string sh_coef_file ="..//data//sh_coefficient.txt";
		string objfile = "..//data//model.obj";

		Application app(cube_textures, sh_coef_file, objfile);
		app.SetWindowSize(800, 600);
		app.SetWindowTitle("Spherical Harmonics Lighting");
		app.Run();
	}
	catch (exception e){
		cerr << e.what() << endl;
	}
	return 0;
}