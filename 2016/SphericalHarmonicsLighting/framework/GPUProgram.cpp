#include "GPUProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
//#include "GameFramework/StdAfx.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	GPUProgram::GPUProgram()
	{

	}

	GPUProgram::~GPUProgram()
	{
		// -----------------ɾ����ɫ���������-----------------
		glDeleteProgram(m_uiProgramID);
	}

	void GPUProgram::AddShader(
		GLenum SHADER_TYPE, const std::string& sFileName)
	{
		m_mapShaderFileName.insert(std::make_pair(SHADER_TYPE, sFileName));
	}

	GLuint GPUProgram::CreateGPUProgram()
	{
		if (GLEW_OK != glewInit())
		{
			std::cerr << "Unable to initialize GLEW... Exiting..." << std::endl;
			std::exit(EXIT_FAILURE);
		}

		// -----------------������ɫ���������-----------------
		m_uiProgramID = glCreateProgram();

		for (auto aPair : m_mapShaderFileName)
		{
			// -----------------������ɫ������-----------------
			GLuint shader = glCreateShader(aPair.first);

			// -----------------��ȡ��ɫ�������ļ�����-----------------
			std::string sShaderSrc = _ReadShaderSourceCode(aPair.second);

			// -----------------������ɫ���������ɫ��Դ����-----------------
			const GLchar *src = const_cast<GLchar *>(sShaderSrc.c_str());
			glShaderSource(shader, 1, &src, NULL);

			// -----------------������ɫ��Դ����-----------------
			_CompileShader(shader);

			// -----------------������ɫ��������ɫ���������-----------------
			glAttachShader(m_uiProgramID, shader);
		}

		// ----------------������ɫ���������-----------------
		_LineGPUProgram();

		return m_uiProgramID;
	}

	std::string GPUProgram::_ReadShaderSourceCode(const std::string& sFileName) const
	{
		std::ifstream is;
		is.open(sFileName);

		std::stringstream ss;
		ss << is.rdbuf();

		return ss.str();
	}

	void GPUProgram::_RecordShaderLog(GLuint shader) const
	{
		// -----------------��ȡLog����----------------
		GLsizei len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		// -----------------��ȡLog����ӡ----------------
		GLchar* LOG = new GLchar[len + 1];
		glGetShaderInfoLog(shader, len, &len, LOG);
		std::cerr << "Program link failed: " << LOG << std::endl;
		delete[] LOG;
	}

	void GPUProgram::_CompileShader(GLuint shader) const
	{
		glCompileShader(shader);

		GLint compileSucceed;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceed);
		if (!compileSucceed)
		{
			_RecordShaderLog(shader);
			throw std::exception("Failed to compile the shader!");
		}
	}

	void GPUProgram::_LineGPUProgram() const
	{
		glLinkProgram(m_uiProgramID);

		GLint linkSucceed;
		glGetProgramiv(m_uiProgramID, GL_LINK_STATUS, &linkSucceed);
		if (!linkSucceed)
		{
			_RecordProgramLog();
			throw std::exception("Failed to link the GPUProgram!");
		}
	}

	void GPUProgram::_RecordProgramLog() const
	{
		// -----------------��ȡLog����----------------
		GLsizei len;
		glGetProgramiv(m_uiProgramID, GL_INFO_LOG_LENGTH, &len);

		// -----------------��ȡLog����ӡ----------------
		std::shared_ptr<GLchar> LOG(
			new GLchar[len + 1], std::default_delete<GLchar>());
		glGetProgramInfoLog(m_uiProgramID, len, &len, LOG.get());
		std::cerr << "Program link failed: " << *LOG << std::endl;
	}

#ifdef __cplusplus
}
#endif // __cplusplus