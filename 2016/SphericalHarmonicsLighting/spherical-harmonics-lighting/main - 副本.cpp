#include <array>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphics.h"
#include "framework.h"
#include "geometry.h"
#include "inputs.h"
#include <FreeImage.h>
#include "macro.h"

using namespace std;
struct float3 { float r, g, b; };
class SHLightingApp
	:public fw::Application{
public:
	SHLightingApp(array<string, 6> cube_textures, string sh_coef_file, string objfile)
		:cube_textures_(cube_textures), sh_coef_file_(sh_coef_file), objfile_(objfile)
	{}

private:
	// parameters
	array<string, 6> cube_textures_;
	string sh_coef_file_;
	string objfile_;

	// objects
	fw::SkyBox* skybox_;
	shared_ptr<fw::Model> model_;
	GLuint model_program_;
	glm::vec3 coefs[16];

	fw::ObserverInput* input_proc_;
	
	glm::mat4 view, proj;
	GLuint skybox_textures_;

	void OnInit() override;
	void OnUpdate(float dt) override;
	void OnShutdown() override;
};

void SHLightingApp::OnInit()
{
	// load parameters
	ifstream ifs(sh_coef_file_);
	if (!ifs)
		throw runtime_error("open " + sh_coef_file_ + " failed");
	for (int i = 0; i < 16; i++){
		ifs >> coefs[i].r >> coefs[i].g >> coefs[i].b;
	}

	// input proc
	input_proc_ = new fw::ObserverInput({ 3.f, 3.f, 3.f }, { 0.f, 1.f, 0.f });
	this->SetInputProcessor(input_proc_);

	// sky box
	skybox_ = new fw::SkyBox(cube_textures_);

	// object
	model_ = fw::LoadModel(objfile_);	
	model_program_ = fw::CreateProgram("shaders/sh_lighting_vert.glsl", "shaders/sh_lighting_frag.glsl");

	// setup opengl
	glViewport(0, 0, WindowWidth(), WindowHeight());
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

}

void SHLightingApp::OnUpdate(float dt)
{
	glEnable(GL_DEPTH_TEST);

	view = input_proc_->GetCameraView();
	proj = glm::perspective(glm::radians(60.f), FrameRatio(), 0.1f, 100.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	skybox_->Draw(proj*view);
	glUseProgram(model_program_);
	// compute transforms
	glm::mat4 model_trans = input_proc_->GetModelTransform();
	glm::mat4 model_view_proj = proj*view*model_trans;
	glm::mat4 normal_trans = glm::transpose(glm::inverse(model_trans));

	// delivery transforms
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "normal_trans"),
		1, false, glm::value_ptr(normal_trans));
	glUniform3fv(glGetUniformLocation(model_program_, "coef"),
		16, (float*)(coefs));
	model_->Draw(model_program_);

}

void SHLightingApp::OnShutdown()
{
	delete input_proc_;
	delete skybox_;
	glDeleteTextures(1, &skybox_textures_);
	glDeleteProgram(model_program_);
}

int main(int argc, char *argv[])
{
	try {
		/*if (argc < 9)
			throw runtime_error("Usage: sh_lighting posx negx posy negy posz negz coefficients.txt model");*/
		array<string, 6> cube_textures;
		//for (int i = 0; i < 6; i++)
		/*cube_textures[0] = "..//data//posx.jpg";
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


		string objfile = "..//data//PokemonBall.obj";

		int rows, cols;
		string envMap = "..//data//panorama_map.jpg";
		FREE_IMAGE_FORMAT fit = FreeImage_GetFileType(envMap.c_str());
		FIBITMAP* img = FreeImage_Load(fit, envMap.c_str());
		if (!img)
			throw runtime_error("load " + envMap + " failed");

		cols = FreeImage_GetWidth(img);
		rows = FreeImage_GetHeight(img);

		std::vector<glm::vec3> coeffs;
		for (int i = 0; i < 9; i++) {
			coeffs.push_back(glm::vec3(0, 0, 0));
		}
		
		const int ntheta = rows;
		const int nphi = cols;
		if (std::min(ntheta, nphi) > 50u) {
			float *buf = new float[2 * ntheta + 2 * nphi];
			float *bufp = buf;
			float *sintheta = bufp;  bufp += ntheta;
			float *costheta = bufp;  bufp += ntheta;
			float *sinphi = bufp;    bufp += nphi;
			float *cosphi = bufp;

			for (int theta = 0; theta < ntheta; ++theta) {
				sintheta[theta] = sinf((theta + .5f) / ntheta * M_PIf);
				costheta[theta] = cosf((theta + .5f) / ntheta * M_PIf);
			}
			for (int phi = 0; phi < nphi; ++phi) {
				sinphi[phi] = sinf((phi + .5f) / nphi * M_2PIf);
				cosphi[phi] = cosf((phi + .5f) / nphi * M_2PIf);
			}

			// SH calculation;
		// TODO:拷贝过程可以优化
			const float F{ M_2PIf / ntheta * M_2PIf / nphi };
			for (int theta = 0; theta < ntheta; ++theta) {
				for (int phi = 0; phi < nphi; ++phi) {
					// Add _InfiniteAreaLight_ texel's contribution to SH coefficients
					// 左手坐标系，z轴向上；
					
					glm::vec3 w{ sintheta[theta] * cosphi[phi], sintheta[theta] * sinphi[phi], costheta[theta] };
					/******************************************************************************/
					// 上句 w在构建的时候就是单位化的，所以注释掉下面的；
					//  w = Normalize(LightToWorld(w));
					/******************************************************************************/
					RGBQUAD c;
					// = make_float3(imgData[theta*nphi + phi]);
					FreeImage_GetPixelColor(img, theta*nphi, phi, &c);
					const glm::vec3 Le = { c.rgbRed,c.rgbGreen,c.rgbBlue };
					coeffs[0] += Le * Y00*sintheta[theta] * F;
					coeffs[1] += Le * Y1_1(w.r)*sintheta[theta] * F;
					coeffs[2] += Le * Y10(w.b)*sintheta[theta] * F;
					coeffs[3] += Le * Y11(w.r)*sintheta[theta] * F;
					coeffs[4] += Le * Y2_2(w.r, w.g)*sintheta[theta] * F;
					coeffs[5] += Le * Y2_1(w.g, w.b)*sintheta[theta] * F;
					coeffs[6] += Le * Y20(w.b)*sintheta[theta] * F;
					coeffs[7] += Le * Y21(w.r, w.b)*sintheta[theta] * F;
					coeffs[8] += Le * Y22(w.r, w.g)*sintheta[theta] * F;
				}
			}
			coeffs[0] *= A0;
			coeffs[1] *= A1;
			coeffs[2] *= A1;
			coeffs[3] *= A1;
			coeffs[4] *= A2;
			coeffs[5] *= A2;
			coeffs[6] *= A2;
			coeffs[7] *= A2;
			coeffs[8] *= A2;
		}
		string outputfile = "..//data//env.txt";
		

		std::array<glm::vec3, 9> coefficients;
		for (int i = 0; i < 9;i++) {
			coefficients[i] = coeffs[i];
		}
		ofstream ofs(outputfile);
		for (auto c : coefficients) {
			ofs << c.x << '\t' << c.y << '\t' << c.z;
			cout << c.r << '\t' << c.g << '\t' << c.b;
		}
		cout << "written to " + outputfile << endl;

		string sh_coef_file = "..//data//sh_coefficient.txt";

		SHLightingApp app(cube_textures, sh_coef_file, objfile);
		app.SetWindowSize(800, 600);
		app.SetWindowTitle("Spherical Harmonics Lighting");
		app.Run();
	}
	catch (exception e){
		cerr << e.what() << endl;
	}
	return 0;
}