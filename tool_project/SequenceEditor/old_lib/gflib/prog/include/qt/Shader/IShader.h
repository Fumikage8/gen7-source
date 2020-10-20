#ifndef GF_GRAPHICS_EFFECT_IShader_H_
#define GF_GRAPHICS_EFFECT_IShader_H_

#include "Type.h"

namespace gfl { namespace maya { namespace GLInterface {

	//-----------------------------------------------------------------------------
	/**
	 * @class	IShader
	 * @brief	シェーダーインターフェイス
	 */
	class IShader{
	public:
		virtual ~IShader(){}

		virtual ShaderType::Enum vGetShaderType() const = 0;
		virtual void vSetLinkProgram( ShaderObjectHandle program_object_id ) = 0;
		virtual ShaderProgramHandle vGetProgramHandle() const = 0;
		virtual void vSetUpShader() const = 0;

	protected:
		IShader(){}
	};

}}}

#endif
