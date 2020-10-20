#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLPixelShader.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLPixelShader::WinOpenGLPixelShader( const void* code, u32 size ) : Shader()
{
  (void)size;

  // #versionを削除する
  std::string str_version = "#version 430";
  std::string str_code = (const char*)code;
  std::string::size_type pos = 0;
  while (pos = str_code.find(str_version, pos), pos != std::string::npos)
  {
    str_code.replace(pos, str_version.length(), "");
  }

  // フラグメントシェーダープログラム作成
  const char* fileList[] =
  {
    "#version 430 \n",
    str_code.c_str(),
  };
	m_hPsProgram = glCreateShaderProgramv(GL_FRAGMENT_SHADER, sizeof(fileList) / sizeof(fileList[0]), fileList);

  GLint isLinked = 0;
  glGetProgramiv(m_hPsProgram, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetProgramiv(m_hPsProgram, GL_INFO_LOG_LENGTH, &maxLength);

    static const u32 MAX_PROGRAM_LOG_LENGTH = 1024 * 10;
    GLchar infoLog[MAX_PROGRAM_LOG_LENGTH];
    if (maxLength > MAX_PROGRAM_LOG_LENGTH)
    {
      maxLength = MAX_PROGRAM_LOG_LENGTH;
    }
    glGetProgramInfoLog(m_hPsProgram, maxLength, &maxLength, infoLog);

    OutputDebugStringA(infoLog);
		GFL_ASSERT( 0 );
  }

	{
    GLuint																	handle = m_hPsProgram;

		//テクスチャユニット設定
		glProgramUniform1i( handle, glGetUniformLocation( handle, "Texture[0]"), 0 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "Texture[1]"), 1 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "Texture[2]"), 2 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "CubeTexture[0]"), 3 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "CubeTexture[1]"), 4 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "CubeTexture[2]"), 5 );

		glProgramUniform1i( handle, glGetUniformLocation( handle, "LutTexture[0]"), 9 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "LutTexture[1]"), 10 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "LutTexture[2]"), 11 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "LutTexture[3]"), 12 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "LutTexture[4]"), 13 );
		glProgramUniform1i( handle, glGetUniformLocation( handle, "LutTexture[5]"), 14 );

		glProgramUniform1i( handle, glGetUniformLocation( handle, "HighLightTexture"), 8 );
	}
}

WinOpenGLPixelShader::~WinOpenGLPixelShader()
{
	glDeleteProgram( m_hPsProgram );
}

void WinOpenGLPixelShader::Bind() const
{
}

}}}
