#include "gfl_UtilShader.h"
#include <debug/gfl_Assert.h>
#include <grp/gfl_GraphicsType.h>

namespace gfl { namespace grp { namespace util {

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderObject::ShaderObject() :
	m_ProgramId( 0 ),
	m_VertexShaderID( 0 ),
	m_GeometryShaderID( 0 ),
	m_pPixelShader( NULL ),
	m_VsShaderId( 0 ),
	m_PsShaderId( 0 ),
	m_GShaderId( 0 )
{
	
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderObject::~ShaderObject()
{
	//this->DeleteProgram();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
GLuint ShaderObject::CreateVertexShaderFromMemory( const void* pSrc, size_t fileSize )
{
	GLuint	shaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderBinary(1, &shaderId, GL_PLATFORM_BINARY_DMP, pSrc, fileSize);
	return shaderId;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderObject::CreateProgram()
{
	m_ProgramId = glCreateProgram();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderObject::DeleteProgram()
{
	if ( m_VertexShaderID )
	{
		glDetachShader(m_ProgramId, m_VertexShaderID);
	}

	if ( m_GeometryShaderID )
	{
		glDetachShader(m_ProgramId, m_GeometryShaderID);
	}

	if ( m_pPixelShader )
	{
		glDetachShader( m_ProgramId, m_pPixelShader->vGetObjectID() );
	}

	if ( m_ProgramId )
	{
		glUseProgram(0);
		glDeleteProgram( m_ProgramId );
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderObject::AttachVertexShader( GLuint shader )
{
	m_VertexShaderID = shader;
	glAttachShader(m_ProgramId, m_VertexShaderID);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderObject::AttachGeometryShader( GLuint shader )
{
	m_GeometryShaderID = shader;
	glAttachShader(m_ProgramId, m_GeometryShaderID);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderObject::AttachPixelShader( IShader* pShader )
{
	m_pPixelShader = pShader;
	//glAttachShader( m_ProgramId, m_pPixelShader->vGetObjectID() );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderObject::BindAttribLocation( int index, const char* param_name)
{
	glBindAttribLocation( m_ProgramId, index, param_name );
}

//-----------------------------------------------------------------------------
/// @brief	シェーダー設定完了後リンクして閉じる
//-----------------------------------------------------------------------------
void ShaderObject::LinkProgram()
{
	glLinkProgram(m_ProgramId);
	glValidateProgram(m_ProgramId);

	if ( m_pPixelShader )
	{
		m_pPixelShader->vSetLinkProgram( m_ProgramId );
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderParameterHandle ShaderObject::GetParameterByName(const char* name) const
{
	return glGetUniformLocation( m_ProgramId, name );
}

}}}
