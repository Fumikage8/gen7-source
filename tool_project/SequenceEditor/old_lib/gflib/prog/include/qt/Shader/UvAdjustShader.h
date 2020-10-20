#ifndef _GF_MAYA_PLUGINS_SHADER_UvAdjustSHADER_H_
#define _GF_MAYA_PLUGINS_SHADER_UvAdjustSHADER_H_

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

//-----------------------------------------------------------------------------
/**
 * @class	UvAdjustVertexShader
 * @brief	シェーダークラス
 */
class UvAdjustVertexShader : public GLInterface::IShader
{
public:

	static void printShaderInfoLog( GLuint shader );

	UvAdjustVertexShader();
	UvAdjustVertexShader( Connection::Enum type );

	~UvAdjustVertexShader(){}
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
 * @class	UvAdjustGeometryShader
 * @brief	シェーダークラス
 */
class UvAdjustGeometryShader : public GLInterface::IShader
{
public:
	UvAdjustGeometryShader();
	~UvAdjustGeometryShader(){}
	virtual GLInterface::ShaderType::Enum vGetShaderType() const	{ return GLInterface::ShaderType::Geometry; }
	virtual void vSetLinkProgram( GLInterface::ShaderObjectHandle h_object );
	virtual GLInterface::ShaderProgramHandle vGetProgramHandle() const { return m_hProgram; }
	virtual void vSetUpShader() const;

	GLInterface::ShaderProgramHandle		m_hProgram;
	GLInterface::ShaderObjectHandle			m_hParentShaderObject;
};

//-----------------------------------------------------------------------------
/**
 * @class	UvAdjustPixelShader
 * @brief	シェーダークラス
 */
class UvAdjustPixelShader : public GLInterface::IShader
{
public:
	UvAdjustPixelShader();
	UvAdjustPixelShader( void* pParam );
	~UvAdjustPixelShader(){}
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
