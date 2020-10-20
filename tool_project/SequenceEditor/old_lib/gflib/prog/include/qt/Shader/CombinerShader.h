#ifndef _GF_MAYA_PLUGINS_SHADER_COMBINERSHADER_H_
#define _GF_MAYA_PLUGINS_SHADER_COMBINERSHADER_H_

#include "ShaderObject.h"

#include <map>
#include <string>
#include <algorithm>

#include "CombinerAttribute.h"

#define DECLARE_ENUM_BEGIN( CLASSNAME )									\
	class CLASSNAME {													\
	public:																\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )									\
		};																\
	};

namespace gfl { namespace maya { namespace plugins { namespace Shader {

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief つながり
	DECLARE_ENUM_BEGIN( Connection )

		PixelShader = 0,
		GeometryShader,
		NumberOf

	DECLARE_ENUM_END( Connection )

//-----------------------------------------------------------------------------
/**
 * @class	CombinerVertexShader
 * @brief	シェーダークラス
 */
class CombinerVertexShader : public GLInterface::IShader
{
public:

	static void printShaderInfoLog( GLuint shader );

	CombinerVertexShader();
	CombinerVertexShader( Connection::Enum type );

	~CombinerVertexShader(){}
	virtual GLInterface::ShaderType::Enum vGetShaderType() const	{ return GLInterface::ShaderType::Vertex; }
	virtual void vSetLinkProgram( GLInterface::ShaderObjectHandle h_object ){ this->m_hParentShaderObject = h_object; }
	virtual GLInterface::ShaderProgramHandle vGetProgramHandle() const { return m_hProgram; }
	virtual void vSetUpShader() const{}

private:
	GLInterface::ShaderProgramHandle			m_hProgram;
	GLInterface::ShaderObjectHandle			m_hParentShaderObject;

	void createVertexShader( Connection::Enum type = Connection::PixelShader );
};

//-----------------------------------------------------------------------------
/**
 * @class	CombinerPixelShader
 * @brief	シェーダークラス
 */
class CombinerPixelShader : public GLInterface::IShader
{
public:
	CombinerPixelShader();
	CombinerPixelShader( void* pParam );
	~CombinerPixelShader(){}
	virtual GLInterface::ShaderType::Enum vGetShaderType() const	{ return GLInterface::ShaderType::Pixel; }
	virtual void vSetLinkProgram( GLInterface::ShaderObjectHandle h_object ){ this->m_hParentShaderObject = h_object; }
	virtual GLInterface::ShaderProgramHandle vGetProgramHandle() const { return m_hProgram; }
	virtual void vSetUpShader() const;

private:

	GLInterface::ShaderProgramHandle			m_hProgram;
	GLInterface::ShaderObjectHandle			m_hParentShaderObject;

	void createPixelShader( void *pParam = NULL );
};
 
}}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif
