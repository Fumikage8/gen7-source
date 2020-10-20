#ifndef GF_GRAPHICS_EFFECT_WINGLSLShaderObject_H_
#define GF_GRAPHICS_EFFECT_WINGLSLShaderObject_H_

#include <qt/Shader/IShader.h>

namespace gfl { namespace maya { namespace GLInterface {

	class IShader;
	
	namespace Win {

	//-----------------------------------------------------------------------------
	/**
	 * @class	WinGLSLShaderObject
	 * @brief	シェーダークラス
	 */
	class WinGLSLShaderObject
	{
	public:
		WinGLSLShaderObject();
		virtual ~WinGLSLShaderObject();

	protected:
		void attachShader( IShader* pShader );
		void linkProgram();

		ShaderParameterHandle getParameterByName(const char* name) const;
		void setVector(const ShaderParameterHandle& parameter, const gfl::maya::Math::Vector& vector);
		void setVectorArray(const ShaderParameterHandle& parameter, const gfl::maya::Math::Vector& vector, UInt32 count);
		void setMatrix(const ShaderParameterHandle& parameter, const gfl::maya::Math::Matrix& matrix, UInt32 count);

		ShaderObjectHandle getObjectHandle() const;
		void setShader() const;

		IShader								*m_pIShader[ShaderType::NumberOf];
		ShaderObjectHandle		m_hObject;
	};

	

}}}}

#endif

