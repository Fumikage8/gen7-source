#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLGeometryShader.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLGeometryShader::WinOpenGLGeometryShader( const void* code, u32 size ) : Shader()
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

  // ジオメトリシェーダープログラム作成
  const char* fileList[] =
  {
    "#version 430 \n",
    str_code.c_str(),
  };
	m_hGsProgram = glCreateShaderProgramv(GL_GEOMETRY_SHADER, sizeof(fileList) / sizeof(fileList[0]), fileList);

  GLint isLinked = 0;
  glGetProgramiv(m_hGsProgram, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetProgramiv(m_hGsProgram, GL_INFO_LOG_LENGTH, &maxLength);

    static const u32 MAX_PROGRAM_LOG_LENGTH = 1024 * 10;
    GLchar infoLog[MAX_PROGRAM_LOG_LENGTH];
    if (maxLength > MAX_PROGRAM_LOG_LENGTH)
    {
      maxLength = MAX_PROGRAM_LOG_LENGTH;
    }
    glGetProgramInfoLog(m_hGsProgram, maxLength, &maxLength, infoLog);

    OutputDebugStringA(infoLog);
		GFL_ASSERT( 0 );
  }
}

WinOpenGLGeometryShader::~WinOpenGLGeometryShader()
{
	glDeleteProgram( m_hGsProgram );
}

void WinOpenGLGeometryShader::Bind() const
{
}

}}}
