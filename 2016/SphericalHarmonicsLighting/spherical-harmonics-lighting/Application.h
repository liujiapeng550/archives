#pragma once
#include <array>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <fstream>
#include <sstream>
#include <memory>
#include "graphics.h"
#include "framework.h"
#include "geometry.h"
#include "inputs.h"
#include "filesystem.h"
using namespace std;

class Application
	:public fw::Application {
public:
	Application(array<string, 6> cube_textures, string sh_coef_file, string objfile);
	std::string Process(const std::string& src) const;
	bool LoadShaderStr(const char* szShaderPath, string& strShaderStr);
private:
	// parameters
	array<string, 6> cube_textures_;
	string sh_coef_file_;
	string objfile_;

	// objects
	fw::SkyBox* skybox_;
	shared_ptr<fw::Model> model_;
	GLuint model_program_;
	glm::vec3 f_coefs[16];

	fw::ObserverInput* input_proc_;

	glm::mat4 view, proj;
	GLuint skybox_textures_;
	void OutLine(std::string& line, std::ostream& out) const;
	bool Process(std::istream& in, std::ostream& out) const;

	void OnInit() override;
	void OnUpdate(float dt) override;
	void OnShutdown() override;
};