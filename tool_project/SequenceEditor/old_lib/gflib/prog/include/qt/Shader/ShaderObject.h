#ifndef GF_GRAPHICS_EFFECT_ShaderObject_H_
#define GF_GRAPHICS_EFFECT_ShaderObject_H_

#include "IShader.h"

#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <string.h>

#include "NativeMath/Math.h"

#include "Win/WinGLSLShaderObject.h"

namespace gfl { namespace maya { namespace GLInterface {

	//-----------------------------------------------------------------------------
	/**
	 * @class	BaseShaderObject
	 * @brief	シェーダーオブジェクト基底クラス
	 */
	template <class Implement_>
	class BaseShaderObject : public Implement_
	{
	public:

		BaseShaderObject() : Implement_(){}
		virtual ~BaseShaderObject(){}

		void AttachShader( IShader* pShader )
		{
			this->Implement_::attachShader( pShader );
		}

		void LinkProgram()
		{
			this->Implement_::linkProgram();
		}

		ShaderParameterHandle GetParameterByName( const char* message, ... )
		{
			static char tmp[1024];

			va_list list;

			va_start( list, message );
			vsprintf_s( tmp, sizeof(tmp), message, list );
			va_end( list );

			return this->Implement_::getParameterByName(tmp);
		}

		void SetVector(const ShaderParameterHandle& parameter, const gfl::maya::Math::Vector& vector)
		{
			this->Implement_::setVector(parameter, vector);
		}

		void SetVectorArray(const ShaderParameterHandle& parameter, const gfl::maya::Math::Vector& vector, UInt32 count)
		{
			this->Implement_::setVectorArray(parameter, vector, count);
		}

		void SetMatrix(const ShaderParameterHandle& parameter, const gfl::maya::Math::Matrix& matrix, UInt32 count = 1)
		{
			this->Implement_::setMatrix(parameter, matrix, count);
		}

		ShaderObjectHandle GetObjectHandle() const
		{
			return this->Implement_::getObjectHandle();
		}

		void SetShader() const
		{
			return this->Implement_::setShader();
		}

		static inline BaseShaderObject* CreateShaderObject( IShader* pVs, IShader* pGs, IShader *pPs )
		{
			BaseShaderObject			*pShaderObject = DbgNew BaseShaderObject();
			
			if ( pVs )
				pShaderObject->AttachShader( pVs );
			if ( pGs )
				pShaderObject->AttachShader( pGs );
			if ( pPs )
				pShaderObject->AttachShader( pPs );

			pShaderObject->LinkProgram();

			return pShaderObject;
		}
	};

#if defined( PLATFORM_3DS )
	typedef				BaseShaderObject<N3DS::N3DSShaderObject>	ShaderObject;
#elif defined( PLATFORM_WIN )
	typedef				BaseShaderObject<Win::WinGLSLShaderObject>		ShaderObject;
#endif

}}}

#endif
