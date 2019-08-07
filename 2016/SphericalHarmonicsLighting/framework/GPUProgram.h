//////////////////////////////////////////////////////////////////////////
/// \file	GPUProgram.h
///
/// \author	 LI Jihong (mailto:lijihong0723@gmail.com)
///
/// \brief	 class GPUProgram declaration
///
/// \version 1.0
///
/// \date	 2016/04/23
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <string>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include "GL/glew.h"
#include "ResourceCommon.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	/// \brief	
	class GPUProgram
	{
	public:
		/// \brief	constructor & destructor
		RESOURCE_EXPORT GPUProgram();
		RESOURCE_EXPORT ~GPUProgram();

		void RESOURCE_EXPORT AddShader(GLenum SHADER_TYPE, const std::string& sFileName);

		GLuint RESOURCE_EXPORT CreateGPUProgram();

	private:
		std::string _ReadShaderSourceCode(const std::string& sFileName) const;

		void _RecordShaderLog(GLuint shader) const;

		void _CompileShader(GLuint shader) const;

		void _LineGPUProgram() const;

		void _RecordProgramLog() const;

	private:
		std::map<GLenum, std::string> m_mapShaderFileName;

		GLuint m_uiProgramID;
	};

#ifdef __cplusplus
}
#endif // __cplusplus