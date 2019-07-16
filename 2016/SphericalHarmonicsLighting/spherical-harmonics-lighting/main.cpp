#include <array>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include "graphics.h"
#include "framework.h"
#include "geometry.h"
#include "inputs.h"
#include "filesystem.h"
using namespace std;

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
	model_program_ = fw::CreateProgram("shaders/vs.glsl", "shaders/fs.glsl");

	// setup opengl
	glViewport(0, 0, WindowWidth(), WindowHeight());
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


void SHLightingApp::OnUpdate(float dt)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	view = input_proc_->GetCameraView();
	proj = glm::perspective(glm::radians(60.f), FrameRatio(), 0.1f, 600.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	skybox_->Draw(proj*view);
	glUseProgram(model_program_);
	// compute transforms
	glm::mat4 model_trans = input_proc_->GetModelTransform();
	glm::mat4 model_view_proj = proj*view*model_trans;
	glm::mat4 normal_trans = glm::transpose(glm::inverse(model_trans));

	// delivery transforms
	glUniformMatrix4fv(glGetUniformLocation(model_program_,"worldMatrix"),
		1,false,glm::value_ptr(model_trans));
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "worldViewProjMatrix"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "worldInverseTransposeMatrix"),
		1, false, glm::value_ptr(normal_trans));

	glUniformMatrix4fv(glGetUniformLocation(model_program_, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "normal_trans"),
		1, false, glm::value_ptr(normal_trans));
	glUniform3fv(glGetUniformLocation(model_program_, "coef"),
		16, (float*)(coefs));

	//???
	// load textures
	// -------------
	unsigned int base = loadTexture(FileSystem::getPath("C:/Users/Administrator/Pictures/bricks/base.png").c_str());
	unsigned int normal = loadTexture(FileSystem::getPath("C:/Users/Administrator/Pictures/bricks/normal.png").c_str());
	unsigned int height = loadTexture(FileSystem::getPath("C:/Users/Administrator/Pictures/bricks/height.png").c_str());
	unsigned int metallic = loadTexture(FileSystem::getPath("C:/Users/Administrator/Pictures/bricks/metallic.png").c_str());
	unsigned int roughness = loadTexture(FileSystem::getPath("C:/Users/Administrator/Pictures/bricks/roughness.png").c_str());

	glUniform1i(glGetUniformLocation(model_program_, "baseColorMap"), 0);
	glUniform1i(glGetUniformLocation(model_program_, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(model_program_, "heightMap"), 2);
	glUniform1i(glGetUniformLocation(model_program_, "metallicMap"), 3);
	glUniform1i(glGetUniformLocation(model_program_, "roughnessMap"), 4);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, base);
	// bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	// bind specular map
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, height);
	// bind specular map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, metallic);
	// bind specular map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, roughness);
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

		//cube_textures[0] = "..//data//right.jpg";
		//cube_textures[1] = "..//data//left.jpg";
		//cube_textures[2] = "..//data//top.jpg";
		//cube_textures[3] = "..//data//bottom.jpg";
		//cube_textures[4] = "..//data//front.jpg";
		//cube_textures[5] = "..//data//back.jpg";

		cube_textures[0] = "..//data//Soft_1light_cuberight.tif";
		cube_textures[1] = "..//data//Soft_1light_cubeleft.tif";
		cube_textures[2] = "..//data//Soft_1light_cubetop.tif";
		cube_textures[3] = "..//data//Soft_1light_cubebottom.tif";
		cube_textures[4] = "..//data//Soft_1light_cubefront.tif";
		cube_textures[5] = "..//data//Soft_1light_cubeback.tif";

		string sh_coef_file ="..//data//Soft.txt";
		string objfile = "..//data//Cube_obj.obj";

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