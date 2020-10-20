#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLVertexShader.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLVertexShader::WinOpenGLVertexShader( const void* code, u32 size, const VertexElement* pVertexElements ) : Shader()
{
  (void)size;

	for( s32 i = 0; i < s_VertexShaderInputMax; ++i )
  {
		m_Strides[i] = 0;
    m_StreamNoList[i] = 0xFFFFFFFF;
  }

  // #versionを削除する
  std::string str_version = "#version 430";
  std::string str_code = (const char*)code;
  std::string::size_type pos = 0;
  while (pos = str_code.find(str_version, pos), pos != std::string::npos)
  {
    str_code.replace(pos, str_version.length(), "");
  }

  // 頂点シェーダープログラム作成
  const char* fileList[] =
  {
    "#version 430 \n",
    str_code.c_str(),
  };
  m_hVsProgram = glCreateShaderProgramv(GL_VERTEX_SHADER, sizeof(fileList) / sizeof(fileList[0]), fileList);

  GLint isLinked = 0;
  glGetProgramiv(m_hVsProgram, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetProgramiv(m_hVsProgram, GL_INFO_LOG_LENGTH, &maxLength);

    static const u32 MAX_PROGRAM_LOG_LENGTH = 1024 * 10;
    GLchar infoLog[MAX_PROGRAM_LOG_LENGTH];
    if (maxLength > MAX_PROGRAM_LOG_LENGTH)
    {
      maxLength = MAX_PROGRAM_LOG_LENGTH;
    }
    glGetProgramInfoLog(m_hVsProgram, maxLength, &maxLength, infoLog);

    OutputDebugStringA(infoLog);
		GFL_ASSERT( 0 );
  }

  // VAO作成
	static const GLuint s_FormatTable[ VertexType::NumberOf ] = 
  {
		0, GL_FLOAT, GL_HALF_FLOAT, 0, GL_SHORT, 0, GL_UNSIGNED_BYTE, 0, 0
	};

  glGenVertexArrays(1, &m_hVertexArray);
  glBindVertexArray(m_hVertexArray);
	{
		s32		elementCnt = 0;
		s32		dataOffSet = 0;
		s32		beforeInputSlotNo = -1;

		while( pVertexElements->Stream != 0xFF)
		{
			if ( beforeInputSlotNo != pVertexElements->Stream )
				dataOffSet = 0;

      glVertexAttribBinding(elementCnt, pVertexElements->Stream);
      glVertexAttribFormat(elementCnt, pVertexElements->Size, s_FormatTable[pVertexElements->Type], GL_FALSE, dataOffSet);

			dataOffSet += GFGLUtil::GetCoordStride( static_cast<VertexType>(pVertexElements->Type), pVertexElements->Size );
			m_Strides[pVertexElements->Stream] = dataOffSet;
      m_StreamNoList[elementCnt] = pVertexElements->Stream;

			beforeInputSlotNo = pVertexElements->Stream;
			++pVertexElements;
			++elementCnt;
		}
		GFL_ASSERT( elementCnt <= s_VertexShaderInputMax );
	}
  glBindVertexArray(0);
}

WinOpenGLVertexShader::~WinOpenGLVertexShader()
{
  // VAO削除
  glDeleteVertexArrays(1, &m_hVertexArray);

  // プログラム削除
  glDeleteProgram(m_hVsProgram);
}

void WinOpenGLVertexShader::Bind() const
{
  glBindVertexArray(m_hVertexArray);

  // 一旦全インデックスを無効にしておく
  for (u32 i = 0; i < s_VertexShaderInputMax; ++i)
  {
    glDisableVertexAttribArray(i);
  }
}

void WinOpenGLVertexShader::BindVertyexAttribute( const u32 index, const u32 bufferHandle ) const
{
  glBindVertexArray(m_hVertexArray);

  // 頂点バッファをバインドしてインデックスを有効にする
  glBindVertexBuffer(index, bufferHandle, 0, m_Strides[index]);
  for (u32 i = 0; i < s_VertexShaderInputMax; ++i)
  {
    if (m_StreamNoList[i] == index)
    {
      glEnableVertexAttribArray(i);
    }
  }
}

}}}
