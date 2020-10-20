#include "qt/Shader/ShaderObject.h"
#include "qt/Shader/Win/WinGLSLShaderObject.h"

//#include <String/Util.h>

namespace gfl { namespace maya { namespace GLInterface { namespace Win {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				シェーダーオブジェクトクラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
WinGLSLShaderObject::WinGLSLShaderObject()
{
	
	m_hObject = glCreateProgram();
	for( int i = 0; i < ShaderType::NumberOf; ++i )
	{
		m_pIShader[i] = NULL;
	}

	int			n;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n); 

	//以下がシェーダーアトリビュートの規則とする
	glBindAttribLocation(m_hObject, 0, "inPosition");
	glBindAttribLocation(m_hObject, 1, "inColor");
	glBindAttribLocation(m_hObject, 2, "inUv0");
  glBindAttribLocation(m_hObject, 3, "inUv1");
  glBindAttribLocation(m_hObject, 4, "inUv2");
	glBindAttribLocation(m_hObject, 5, "inNormal");
	glBindAttribLocation(m_hObject, 6, "inTangent0");
	glBindAttribLocation(m_hObject, 7, "inTangent1");
	glBindAttribLocation(m_hObject, 8, "inTangent2");
	glBindAttribLocation(m_hObject, 9, "inJointIndex");
	glBindAttribLocation(m_hObject,10, "inJointWeight");
	glBindAttribLocation(m_hObject,11, "inOrigNormal");
	glBindAttribLocation(m_hObject,12, "inOrigTangent");
	
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
WinGLSLShaderObject::~WinGLSLShaderObject()
{
	for( int i = 0; i < ShaderType::NumberOf; ++i )
	{
		if ( m_pIShader[i] )
		{
			glDetachShader( m_hObject, m_pIShader[i]->vGetProgramHandle() );
		}
	}

	if ( m_hObject )
	{
		glUseProgram(0);
		glDeleteProgram( m_hObject );
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void WinGLSLShaderObject::attachShader( IShader* pShader )
{
	
	m_pIShader[ pShader->vGetShaderType() ] = pShader;
	glAttachShader(m_hObject, m_pIShader[ pShader->vGetShaderType() ]->vGetProgramHandle() );
	
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------

/* シェーダリンクエラーの出力 */
static void printProgramInfoLog( GLuint program )
{
	/* シェーダコンパイルのログバッファ */
	#define MAX_SHADER_LOG_SIZE		(1024*10)
	char s_logBuffer[MAX_SHADER_LOG_SIZE];

	int logSize;
	int length;

	/* ログの長さは、最後のNULL文字も含む */
	glGetProgramiv(program, GL_INFO_LOG_LENGTH , &logSize);

	if (logSize > 1)
	{
		glGetProgramInfoLog(program, MAX_SHADER_LOG_SIZE, &length, s_logBuffer);
	}
}

void WinGLSLShaderObject::linkProgram()
{
	for( int i = 0; i < ShaderType::NumberOf; ++i )
	{
		if ( m_pIShader[i] )
		{
			m_pIShader[i]->vSetLinkProgram( m_hObject );
		}
	}

	
	glLinkProgram(m_hObject);
	
	glUseProgram( m_hObject );
	

	GLint linked;
	glGetProgramiv(m_hObject, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		printProgramInfoLog( m_hObject );
	}

	
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderParameterHandle WinGLSLShaderObject::getParameterByName(const char* name) const
{
	return glGetUniformLocation( m_hObject, name );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
UInt32 WinGLSLShaderObject::getObjectHandle() const
{
	return m_hObject;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void WinGLSLShaderObject::setShader() const
{
	glUseProgram( this->m_hObject );

	for( int i = 0; i < ShaderType::NumberOf; ++i )
	{
		if ( m_pIShader[i] )
		{
			m_pIShader[i]->vSetUpShader();
		}
	}	
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void WinGLSLShaderObject::setVector(const ShaderParameterHandle& parameter, const gfl::maya::Math::Vector& vector)
{
	glUniform4fv( parameter, 1, (float*)( &vector ) );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void WinGLSLShaderObject::setVectorArray(const ShaderParameterHandle& parameter, const gfl::maya::Math::Vector& vector, UInt32 count)
{
	glUniform4fv( parameter, count, (float*)( &vector ) );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void WinGLSLShaderObject::setMatrix(const ShaderParameterHandle& parameter, const gfl::maya::Math::Matrix& matrix, UInt32 count)
{
	glUniformMatrix4fv( parameter, count, GL_FALSE, (float*)(&matrix) );
}

}}}}



