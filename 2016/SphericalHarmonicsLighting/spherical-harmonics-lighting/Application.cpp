#include "Application.h"
#include "cubemap.h"
#include "harmonics.h"

Application::Application(array<string, 6> cube_textures, string sh_coef_file, string objfile)
	:cube_textures_(cube_textures),
	sh_coef_file_(sh_coef_file),
	objfile_(objfile)
{
	int degree = 3;
	int samplenum = 100000;
	Harmonics harmonics(degree);
	Cubemap cubemap(cube_textures_);
	auto verticies = cubemap.RandomSample(samplenum);
	harmonics.Evaluate(verticies);
	auto temp = harmonics.getCoefficients();
	std::vector<float> coefs;
	for (const Vec3& c : temp)
	{
		coefs.push_back(c.x);
		coefs.push_back(c.y);
		coefs.push_back(c.z);
	}
	memcpy(f_coefs, coefs.data(), sizeof(f_coefs));
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
void Application::OnInit()
{
	// load parameters
	/*ifstream ifs(sh_coef_file_);
	if (!ifs)
		throw runtime_error("open " + sh_coef_file_ + " failed");
	for (int i = 0; i < 16; i++) {
		ifs >> f_coefs[i].r >> f_coefs[i].g >> f_coefs[i].b;
	}*/

	// input proc
	input_proc_ = new fw::ObserverInput({ 3.f, 3.f, 3.f }, { 0.f, 1.f, 0.f });
	this->SetInputProcessor(input_proc_);

	// sky box
	skybox_ = new fw::SkyBox(cube_textures_);

	// object
	model_ = fw::LoadModel(objfile_);
	//shader Preprocess
	const char* vsPath = "shaders/vs.glsl";
	const char* fsPath = "shaders/fs.glsl";
	std::string fsOut, vsOut;
	LoadShaderStr(fsPath, fsOut);
	fsOut = Process(fsOut);
	LoadShaderStr(vsPath, vsOut);

	const GLchar *v = const_cast<GLchar *>(vsOut.c_str());
	const GLchar *f = const_cast<GLchar *>(fsOut.c_str());
	auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &v, NULL);
	glCompileShader(vertex_shader);
	auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &f, NULL);
	glCompileShader(fragment_shader);

	auto program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	model_program_ = program;


	//???
	// load textures
	// -------------
	GLuint envCube = fw::LoadCubemap(cube_textures_);
	glBindVertexArray(envCube);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(model_program_, "environmentMap"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCube);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	unsigned int base = loadTexture(FileSystem::getPath("data/bricks/1.png").c_str());
	unsigned int normal = loadTexture(FileSystem::getPath("data/bricks/normal.png").c_str());
	unsigned int height = loadTexture(FileSystem::getPath("data/bricks/height.png").c_str());
	unsigned int metallic = loadTexture(FileSystem::getPath("data/bricks/metallic.png").c_str());
	unsigned int roughness = loadTexture(FileSystem::getPath("data/bricks/roughness.png").c_str());

	glUniform1i(glGetUniformLocation(model_program_, "baseColorMap"), 0);
	glUniform1i(glGetUniformLocation(model_program_, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(model_program_, "heightMap"), 2);
	glUniform1i(glGetUniformLocation(model_program_, "metallicMap"), 3);
	glUniform1i(glGetUniformLocation(model_program_, "roughnessMap"), 4);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, base);
	// bind specular map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normal);
	// bind specular map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, height);
	// bind specular map
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, metallic);
	// bind specular map
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, roughness);

	// setup opengl
	glViewport(0, 0, WindowWidth(), WindowHeight());
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

}



void Application::OnUpdate(float dt)
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
	glm::mat4 model_view_proj = proj * view*model_trans;
	glm::mat4 normal_trans = glm::transpose(glm::inverse(model_trans));

	// delivery transforms
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "worldMatrix"),
		1, false, glm::value_ptr(model_trans));
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "worldViewProjMatrix"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "worldInverseTransposeMatrix"),
		1, false, glm::value_ptr(normal_trans));

	glUniformMatrix4fv(glGetUniformLocation(model_program_, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(glGetUniformLocation(model_program_, "normal_trans"),
		1, false, glm::value_ptr(normal_trans));
	glUniform3fv(glGetUniformLocation(model_program_, "shCoefs"),
		16, (float*)(f_coefs));

	model_->Draw(model_program_);

}

void Application::OnShutdown()
{
	delete input_proc_;
	delete skybox_;
	glDeleteTextures(1, &skybox_textures_);
	glDeleteProgram(model_program_);
}

void Application::OutLine(std::string& line, std::ostream& out) const
{
	if (true)
	{
		std::string::size_type s = line.find("//");
		if (s != std::string::npos)
		{
			if (line.length() > s + 2 && '/' == line[s + 2])
			{
				line.erase(s, 1);
			}
			else
			{
				line.resize(s);
			}
			std::string::size_type i = line.length();
			for (; i > 0; --i)
			{
				const std::string::value_type& c = line[i - 1];
				if ('\t' != c && ' ' != c)
				{
					break;
				}
			}
			if (i == 0)
			{
				return;
			}
			line.resize(i);
		}
	}
	out << line << std::endl;
}

bool Application::Process(std::istream& in, std::ostream& out) const
{
	std::string line;
	for (size_t l = 1; !in.eof(); ++l)
	{
		getline(in, line);
		std::string::size_type s = line.find("#include");
		if (std::string::npos == s)
		{
			goto OutOfInclude;
		}
		std::string::size_type d = line.find("\"", s + 8);
		if (std::string::npos == d)
		{
			goto OutOfInclude;
		}
		std::string::size_type e = line.find("\"", d + 1);
		if (std::string::npos == e)
		{
			goto OutOfInclude;
		}
		for (std::string::size_type i = 0; i < s; ++i)
		{
			std::string::value_type c = line.at(i);
			if ('\t' != c && ' ' != c)
			{
				goto OutOfInclude;
			}
		}
		for (std::string::size_type i = s + 8; i < d; ++i)
		{
			std::string::value_type c = line.at(i);
			if ('\t' != c && ' ' != c)
			{
				goto OutOfInclude;
			}
		}
		{
			std::string path = line.substr(d + 1, e - d - 1);
			if (0 == path.length())
			{
				/*if (!!f_ErrorHandle)
				{
					f_ErrorHandle(MessageType::mtError, __FILE__, __LINE__, "无法获取#include路径");
				}*/
				return false;
			}
			std::shared_ptr<std::istream> inf(new std::ifstream(path));
			if (!(*inf))
			{
				/*if (!!f_ErrorHandle)
				{
					std::string msg = "无法找到Include文件" + path;
					f_ErrorHandle(MessageType::mtError, __FILE__, __LINE__, msg.c_str());
				}*/
				return false;
			}

			for (; !inf->eof();)
			{
				getline(*inf, line);
				OutLine(line, out);
			}
		}

		/*if (f_AddLineNumber)
		{
			out << "#line " << l << std::endl;
		}*/
		continue;
	OutOfInclude:
		OutLine(line, out);
	}
	return true;
}

std::string Application::Process(const std::string& src) const
{
	std::ostringstream out;
	std::stringstream in(src);

	Process(in, out);

	return out.str();
}

bool Application::LoadShaderStr(const char* szShaderPath, string& strShaderStr)
{
	if (NULL == szShaderPath)
		return false;

	std::ifstream iShaderStram(szShaderPath, std::ios::in);
	if (iShaderStram.is_open())
	{
		std::stringstream strTemp;
		strTemp << iShaderStram.rdbuf();
		strShaderStr = strTemp.str();
		iShaderStram.close();
		return true;
	}
	printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", szShaderPath);
	getchar();

	return false;
}